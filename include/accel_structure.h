#ifndef VPATHTRACER_ACCEL_STRUCTURE_H
#define VPATHTRACER_ACCEL_STRUCTURE_H

#include "common.h"
#include "object.h"

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

private:
    // BVH Tree
};

class SAHAccelStructure: public AccelStructure {
public:
    explicit SAHAccelStructure(vector<Object*> &objects);
    bool intersect(const Ray &ray, HitRecord &hitRecord) override;

private:
    // SAH Tree
};

#endif //VPATHTRACER_ACCEL_STRUCTURE_H
