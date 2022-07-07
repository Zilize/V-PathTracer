#ifndef VPATHTRACER_TRIANGLE_H
#define VPATHTRACER_TRIANGLE_H

#include "common.h"
#include "ray.h"
#include "hit_record.h"


class Triangle {
public:
    Triangle() = default;
    Triangle(const vec3 &_v0, const vec3 &_v1, const vec3 &_v2, const vec3 &_n0, const vec3 &_n1, const vec3 &_n2)
            : v0(_v0), v1(_v1), v2(_v2), n0(glm::normalize(_n0)), n1(glm::normalize(_n1)), n2(glm::normalize(_n2)) {}

    bool intersect(const Ray *ray, HitRecord *hitRecord);

private:
    vec3 v0, v1, v2;  // vertices
    vec3 n0, n1, n2;  // normalized normals
};

#endif //VPATHTRACER_TRIANGLE_H
