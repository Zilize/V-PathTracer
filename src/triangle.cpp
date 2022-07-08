#include "triangle.h"

bool Triangle::intersect(const Ray &ray, float &tnear, float &b1, float &b2) {
    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;
    vec3 s1 = cross(ray.direction, e2);
    float det = dot(s1, e1);
    if (det == 0 || det < 0) return false;

    vec3 s = ray.origin - v0;
    b1 = dot(s1, s);
    if (b1 < 0 || b1 > det) return false;

    vec3 s2 = cross(s, e1);
    b2 = dot(ray.direction, s2);
    if (b2 < 0 || b1 + b2 > det) return false;

    float invDet = 1.0f / det;
    tnear = dot(e2, s2) * invDet;
    b1 *= invDet;
    b2 *= invDet;
    return true;
}

vec3 Triangle::getNormal(float b0, float b1, float b2) {
    return b0 * n0 + b1 * n1 + b2 * n2;
}

shared_ptr<Material> Triangle::getMaterial() {
    return material;
}