#include <queue>

#include "accel_structure.h"

AABB unionBound(const AABB &A, const AABB &B) {
    AABB C;
    C.lowerBound = lower(A.lowerBound, B.lowerBound);
    C.upperBound = upper(A.upperBound, B.upperBound);
    return C;
}

float areaBound(const AABB &A) {
    vec3 d = A.upperBound - A.lowerBound;
    return 2.0f * (d.x * d.y + d.y * d.z + d.z * d.x);
}

bool testOverlap(const Ray &ray, const AABB &box) {
    auto tLowerX = (box.lowerBound.x - ray.origin.x) / ray.direction.x;
    auto tUpperX = (box.upperBound.x - ray.origin.x) / ray.direction.x;
    auto tMinX = tLowerX < tUpperX ? tLowerX : tUpperX;
    auto tMaxX = tLowerX > tUpperX ? tLowerX : tUpperX;

    auto tLowerY = (box.lowerBound.y - ray.origin.y) / ray.direction.y;
    auto tUpperY = (box.upperBound.y - ray.origin.y) / ray.direction.y;
    auto tMinY = tLowerY < tUpperY ? tLowerY : tUpperY;
    auto tMaxY = tLowerY > tUpperY ? tLowerY : tUpperY;

    auto tLowerZ = (box.lowerBound.z - ray.origin.z) / ray.direction.z;
    auto tUpperZ = (box.upperBound.z - ray.origin.z) / ray.direction.z;
    auto tMinZ = tLowerZ < tUpperZ ? tLowerZ : tUpperZ;
    auto tMaxZ = tLowerZ > tUpperZ ? tLowerZ : tUpperZ;

    auto tEnter = (tMinX > tMinY ? tMinX : tMinY) > tMinZ ? (tMinX > tMinY ? tMinX : tMinY) : tMinZ;
    auto tExit = (tMaxX < tMaxY ? tMaxX : tMaxY) < tMaxZ ? (tMaxX < tMaxY ? tMaxX : tMaxY) : tMaxZ;

    if (tEnter < tExit && tExit >= 0.0f) return true;
    else return false;
}

NaiveAccelStructure::NaiveAccelStructure(vector<Object *> &objects) {
    for (auto object: objects) {
        container.insert(container.end(), object->primitives.begin(), object->primitives.end());
    }
}

bool NaiveAccelStructure::intersect(const Ray &ray, HitRecord &hitRecord) {
    float tNear = std::numeric_limits<float>::max();
    float b1Near, b2Near;
    int indexNearest = -1;
    for (int i = 0; i < container.size(); ++i) {
        float t, b1, b2;  // barycentric coordinates
        if (container[i].intersect(ray, t, b1, b2) && t < tNear && t > 0) {
            tNear = t;
            b1Near = b1;
            b2Near = b2;
            indexNearest = i;
        }
    }
    if (indexNearest == -1) return false;

    hitRecord.point = ray.origin + tNear * ray.direction;
    hitRecord.normal = container[indexNearest].getNormal(1.0f - b1Near - b2Near, b1Near, b2Near);
    hitRecord.time = tNear;
    hitRecord.material = container[indexNearest].getMaterial();
    return true;
}

TreeNode* BVHAccelStructure::buildBVHTree(vector<int> &triangleIndices) {
    if (triangleIndices.empty()) throw std::runtime_error("TriangleIndices is empty.");
    if (triangleIndices.size() == 1) {
        AABB box = containerAABB[triangleIndices[0]];
        auto *leaf = new TreeNode(box, true, nullptr, nullptr, nullptr, triangleIndices[0], 0.0f);
        return leaf;
    }

    AABB box;
    for (auto triangleIndex: triangleIndices) box = unionBound(box, containerAABB[triangleIndex]);
    float lengthX = box.upperBound.x - box.lowerBound.x;
    float lengthY = box.upperBound.y - box.lowerBound.y;
    float lengthZ = box.upperBound.z - box.lowerBound.z;

    if (lengthX >= lengthY && lengthX >= lengthZ) {
        std::sort(triangleIndices.begin(), triangleIndices.end(), [this](int indexA, int indexB) {
            return container[indexA].getBarycenterX() < container[indexB].getBarycenterX();
        });
    }
    else if (lengthY >= lengthX && lengthY >= lengthZ) {
        std::sort(triangleIndices.begin(), triangleIndices.end(), [this](int indexA, int indexB) {
            return container[indexA].getBarycenterY() < container[indexB].getBarycenterY();
        });
    }
    else {  // lengthZ >= lengthX && lengthZ >= lengthY
        std::sort(triangleIndices.begin(), triangleIndices.end(), [this](int indexA, int indexB) {
            return container[indexA].getBarycenterZ() < container[indexB].getBarycenterZ();
        });
    }

    auto begin = triangleIndices.begin();
    auto middle = triangleIndices.begin() + ((int)triangleIndices.size() / 2);
    auto end = triangleIndices.end();

    auto triangleIndicesLeft = vector<int>(begin, middle);
    auto triangleIndicesRight = vector<int>(middle, end);

    TreeNode *leftChild = buildBVHTree(triangleIndicesLeft);
    TreeNode *rightChild = buildBVHTree(triangleIndicesRight);

    auto *leaf = new TreeNode(box, false, nullptr, leftChild, rightChild, -1, 0.0f);
    return leaf;
}

BVHAccelStructure::BVHAccelStructure(vector<Object *> &objects) {
    for (auto object: objects) {
        container.insert(container.end(), object->primitives.begin(), object->primitives.end());
    }
    vector<int> triangleIndices;
    triangleIndices.reserve(container.size());
    for (int i = 0; i < container.size(); ++i) {
        triangleIndices.emplace_back(i);
        const Triangle &t = container[i];
        AABB box;
        box.lowerBound = lower(lower(t.v0, t.v1), t.v2);
        box.upperBound = upper(upper(t.v0, t.v1), t.v2);

        // Avoid 3D box change to 2D piece
        if (box.lowerBound.x == box.upperBound.x) box.upperBound.x += 0.01f;
        if (box.lowerBound.y == box.upperBound.y) box.upperBound.y += 0.01f;
        if (box.lowerBound.z == box.upperBound.z) box.upperBound.z += 0.01f;

        containerAABB.emplace_back(box);
    }
    root = buildBVHTree(triangleIndices);
}

bool BVHAccelStructure::intersect(const Ray &ray, HitRecord &hitRecord) {
    float tNear = std::numeric_limits<float>::max();
    float b1Near, b2Near;
    int indexNearest = -1;

    stack<TreeNode*> stackTreeNode;
    stackTreeNode.push(root);
    while (!stackTreeNode.empty()) {
        TreeNode *currentTreeNode = stackTreeNode.top();
        stackTreeNode.pop();
        if (!testOverlap(ray, currentTreeNode->box))
            continue;
        if (currentTreeNode->isLeaf) {
            int triangleIndex = currentTreeNode->triangleIndex;
            float t, b1, b2;
            if (container[triangleIndex].intersect(ray, t, b1, b2) && t < tNear && t > 0) {
                tNear = t;
                b1Near = b1;
                b2Near = b2;
                indexNearest = triangleIndex;
            }
        }
        else {
            stackTreeNode.push(currentTreeNode->leftChild);
            stackTreeNode.push(currentTreeNode->rightChild);
        }
    }

    if (indexNearest == -1) return false;

    hitRecord.point = ray.origin + tNear * ray.direction;
    hitRecord.normal = container[indexNearest].getNormal(1.0f - b1Near - b2Near, b1Near, b2Near);
    hitRecord.time = tNear;
    hitRecord.material = container[indexNearest].getMaterial();
    return true;
}

float SAHAccelStructure::ancestorCostDelta(TreeNode *pointer, int leafIndex) {
    if (!pointer) return 0.0f;
    float result = 0.0f;
    while (pointer) {
        result += areaBound(unionBound(pointer->box, containerAABB[leafIndex])) - areaBound(pointer->box);
        pointer = pointer->parent;
    }
    return result;
}

TreeNode *SAHAccelStructure::findBestSibling(int leafIndex) {
    TreeNode *bestSibling = nullptr;
    float bestCost = std::numeric_limits<float>::max();
    std::priority_queue<TreeNode*, vector<TreeNode*>, std::function<bool(TreeNode*, TreeNode*)>> siblingQueue([](TreeNode* A, TreeNode* B) {
        return A->cost > B->cost;
    });
    root->cost = areaBound(unionBound(root->box, containerAABB[leafIndex]));
    siblingQueue.push(root);

    while (!siblingQueue.empty()) {
        TreeNode *currentSibling = siblingQueue.top();
        siblingQueue.pop();
        float currentCost = currentSibling->cost;
        if (currentCost < bestCost) {
            bestSibling = currentSibling;
            bestCost = currentCost;
        }

        if (currentSibling->isLeaf) continue;
        // To see if it is worthwhile to explore the subtree
        float costAncestor = ancestorCostDelta(currentSibling, leafIndex);
        float costLowerBound = areaBound(containerAABB[leafIndex]) + costAncestor;
        if (costLowerBound < bestCost) {
            currentSibling->leftChild->cost = areaBound(unionBound(currentSibling->leftChild->box, containerAABB[leafIndex])) + costAncestor;
            currentSibling->rightChild->cost = areaBound(unionBound(currentSibling->rightChild->box, containerAABB[leafIndex])) + costAncestor;
            siblingQueue.push(currentSibling->leftChild);
            siblingQueue.push(currentSibling->rightChild);
        }
    }
    return bestSibling;
}

void SAHAccelStructure::insertLeaf(int leafIndex) {
    auto *leafNode = new TreeNode(containerAABB[leafIndex], true, nullptr, nullptr,nullptr, leafIndex, 0.0f);
    if (!root) {
        root = leafNode;
        return;
    }

    // Stage 1: find the best sibling for the new leaf
    TreeNode *bestSibling = findBestSibling(leafIndex);

    // Stage 2: create a new parent
    TreeNode *oldParent = bestSibling->parent;
    auto newParent = new TreeNode(unionBound(bestSibling->box, containerAABB[leafIndex]), false, oldParent, nullptr,
                                  nullptr, -1, 0.0f);

    if (oldParent != nullptr) {
        // The sibling was not the root
        if (oldParent->leftChild == bestSibling) oldParent->leftChild = newParent;
        else oldParent->rightChild = newParent;
    }
    else {
        // The sibling was the root
        root = newParent;
    }
    newParent->leftChild = bestSibling;
    newParent->rightChild = leafNode;
    bestSibling->parent = newParent;
    leafNode->parent = newParent;

    // Stage 3: walk back up the tree refitting AABBs
    TreeNode *pointer = oldParent;
    while (pointer) {
        pointer->box = unionBound(pointer->leftChild->box, pointer->rightChild->box);
        pointer = pointer->parent;
    }
}

SAHAccelStructure::SAHAccelStructure(vector<Object *> &objects) {
    // Initialize container and containerAABB
    for (auto object: objects) {
        container.insert(container.end(), object->primitives.begin(), object->primitives.end());
    }
    vector<int> triangleIndices;
    triangleIndices.reserve(container.size());
    for (int i = 0; i < container.size(); ++i) {
        triangleIndices.emplace_back(i);
        const Triangle &t = container[i];
        AABB box;
        box.lowerBound = lower(lower(t.v0, t.v1), t.v2);
        box.upperBound = upper(upper(t.v0, t.v1), t.v2);

        // Avoid 3D box change to 2D piece
        if (box.lowerBound.x == box.upperBound.x) box.upperBound.x += 0.01f;
        if (box.lowerBound.y == box.upperBound.y) box.upperBound.y += 0.01f;
        if (box.lowerBound.z == box.upperBound.z) box.upperBound.z += 0.01f;

        containerAABB.emplace_back(box);
    }

    // Build SAH incrementally
    for (int i = 0; i < container.size(); ++i) insertLeaf(i);
}

bool SAHAccelStructure::intersect(const Ray &ray, HitRecord &hitRecord) {
    float tNear = std::numeric_limits<float>::max();
    float b1Near, b2Near;
    int indexNearest = -1;

    stack<TreeNode*> stackTreeNode;
    stackTreeNode.push(root);
    while (!stackTreeNode.empty()) {
        TreeNode *currentTreeNode = stackTreeNode.top();
        stackTreeNode.pop();
        if (!testOverlap(ray, currentTreeNode->box))
            continue;
        if (currentTreeNode->isLeaf) {
            int triangleIndex = currentTreeNode->triangleIndex;
            float t, b1, b2;
            if (container[triangleIndex].intersect(ray, t, b1, b2) && t < tNear && t > 0) {
                tNear = t;
                b1Near = b1;
                b2Near = b2;
                indexNearest = triangleIndex;
            }
        }
        else {
            stackTreeNode.push(currentTreeNode->leftChild);
            stackTreeNode.push(currentTreeNode->rightChild);
        }
    }

    if (indexNearest == -1) return false;

    hitRecord.point = ray.origin + tNear * ray.direction;
    hitRecord.normal = container[indexNearest].getNormal(1.0f - b1Near - b2Near, b1Near, b2Near);
    hitRecord.time = tNear;
    hitRecord.material = container[indexNearest].getMaterial();
    return true;
}
