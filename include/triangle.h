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

    void sample(vec3 &point, vec3 &normal) const;
    bool intersect(const Ray &ray, float &tNear, float &b1, float &b2) const;
    vec3 getNormal(float b0, float b1, float b2) const;
    shared_ptr<Material> getMaterial() const;
    float getArea() const;

    float getBarycenterX() const { return (v0.x + v1.x + v2.x) / 3.0f; }
    float getBarycenterY() const { return (v0.y + v1.y + v2.y) / 3.0f; }
    float getBarycenterZ() const { return (v0.z + v1.z + v2.z) / 3.0f; }

public:
    vec3 v0, v1, v2;  // vertices
    vec3 n0, n1, n2;  // normalized normals
    shared_ptr<Material> material;
};

#endif //VPATHTRACER_TRIANGLE_H
