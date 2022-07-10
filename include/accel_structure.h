#ifndef VPATHTRACER_ACCEL_STRUCTURE_H
#define VPATHTRACER_ACCEL_STRUCTURE_H

#include "common.h"
#include "object.h"

inline vec3 lower(vec3 a, vec3 b) {
    float minX = a.x < b.x ? a.x : b.x;
    float minY = a.y < b.y ? a.y : b.y;
    float minZ = a.z < b.z ? a.z : b.z;
    return {minX, minY, minZ};
}

inline vec3 upper(vec3 a, vec3 b) {
    float maxX = a.x > b.x ? a.x : b.x;
    float maxY = a.y > b.y ? a.y : b.y;
    float maxZ = a.z > b.z ? a.z : b.z;
    return {maxX, maxY, maxZ};
}

typedef struct AABB {
    vec3 lowerBound;
    vec3 upperBound;
}AABB;

typedef struct TreeNode {
    AABB box;
    bool isLeaf;
    TreeNode *parent;
    TreeNode *leftChild;
    TreeNode *rightChild;
    int triangleIndex;
    float cost{};  // for finding the best sibling in SAH

    TreeNode(AABB _box, bool _isLeaf, TreeNode* _parent, TreeNode *_leftChild, TreeNode *_rightChild, int _triangleIndex):
            box(_box), isLeaf(_isLeaf), parent(_parent), leftChild(_leftChild), rightChild(_rightChild), triangleIndex(_triangleIndex) {}
}TreeNode;

class AccelStructure {
public:
    AccelStructure() = default;
    virtual bool intersect(const Ray &ray, HitRecord &hitRecord) = 0;
};

class NaiveAccelStructure: public AccelStructure {
public:
    explicit NaiveAccelStructure(vector<Object*> &objects);
    bool intersect(const Ray &ray, HitRecord &hitRecord) override;

private:
    vector<Triangle> container;
};

class BVHAccelStructure: public AccelStructure {
public:
    explicit BVHAccelStructure(vector<Object*> &objects);
    bool intersect(const Ray &ray, HitRecord &hitRecord) override;

    TreeNode* buildBVHTree(vector<int> &triangleIndices);

private:
    vector<Triangle> container;
    vector<AABB> containerAABB;
    TreeNode *root;
};

class SAHAccelStructure: public AccelStructure {
public:
    explicit SAHAccelStructure(vector<Object*> &objects);
    bool intersect(const Ray &ray, HitRecord &hitRecord) override;

    float ancestorCostDelta(TreeNode *pointer, int leafIndex);
    TreeNode *findBestSibling(int leafIndex);
    void insertLeaf(int leafIndex);

private:
    vector<Triangle> container;
    vector<AABB> containerAABB;
    TreeNode *root;
};

#endif //VPATHTRACER_ACCEL_STRUCTURE_H
