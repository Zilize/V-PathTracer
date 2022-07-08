#include "accel_structure.h"

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
        if (container[i].intersect(ray, t, b1, b2) && t < tNear) {
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

BVHAccelStructure::BVHAccelStructure(vector<Object *> &objects) {

}

bool BVHAccelStructure::intersect(const Ray &ray, HitRecord &hitRecord) {
    return true;
}

SAHAccelStructure::SAHAccelStructure(vector<Object *> &objects) {

}

bool SAHAccelStructure::intersect(const Ray &ray, HitRecord &hitRecord) {
    return true;
}