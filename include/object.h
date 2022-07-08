#ifndef VPATHTRACER_OBJECT_H
#define VPATHTRACER_OBJECT_H

#include "common.h"
#include "triangle.h"
#include "material.h"

class Object {
public:
    Object() = default;
    Object(const string &filename, shared_ptr<Material> _material);

public:
    vector<Triangle> primitives;
    shared_ptr<Material> material;
};

#endif //VPATHTRACER_OBJECT_H
