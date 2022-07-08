#ifndef VPATHTRACER_TRIANGLE_H
#define VPATHTRACER_TRIANGLE_H

#include <utility>

#include "common.h"
#include "ray.h"
#include "hit_record.h"


class Triangle {
public:
    Triangle(const vec3 &_v0, const vec3 &_v1, const vec3 &_v2, const vec3 &_n0, const vec3 &_n1, const vec3 &_n2, shared_ptr<Material> _material)
            : v0(_v0), v1(_v1), v2(_v2), n0(glm::normalize(_n0)), n1(glm::normalize(_n1)), n2(glm::normalize(_n2)), material(std::move(_material)) {}

    bool intersect(const Ray &ray, float &tnear, float &b1, float &b2);
    vec3 getNormal(float b0, float b1, float b2);
    shared_ptr<Material> getMaterial();

private:
    vec3 v0, v1, v2;  // vertices
    vec3 n0, n1, n2;  // normalized normals
    shared_ptr<Material> material;
};

#endif //VPATHTRACER_TRIANGLE_H
