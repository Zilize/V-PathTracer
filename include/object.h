#ifndef VPATHTRACER_OBJECT_H
#define VPATHTRACER_OBJECT_H

#include "common.h"
#include "triangle.h"

class Object {
public:
    Object() = default;
    explicit Object(const string &filename);

private:
    vector<Triangle> meshes;
};

#endif //VPATHTRACER_OBJECT_H
