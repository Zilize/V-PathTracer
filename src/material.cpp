#include "material.h"

inline vec3 randomCosineDirection() {
    float r1 = glm::linearRand(0.0f, 1.0f);
    float r2 = glm::linearRand(0.0f, 1.0f);
    auto z = glm::sqrt(1 - r2);
    auto phi = 2 * (float)PI * r1;
    auto x = glm::cos(phi) * glm::sqrt(r2);
    auto y = glm::sin(phi) * glm::sqrt(r2);
    return {x, y, z};
}

void buildOrthonormalBasis(const vec3 &normal, vec3 &u, vec3 &v, vec3 &w) {
    // u, v, w is a right-hand basis
    w = normalize(normal);
    vec3 a = (fabsf(w.x) > 0.9f) ? vec3(0,1,0) : vec3(1,0,0);
    v = normalize(cross(w, a));
    u = normalize(cross(v, w));
}

void DiffuseMaterial::reflect(const vec3 &rayInDir, const vec3 &normal, vec3 &rayOutDir, float &pdf) const {
    // Uniform sampling a hemisphere
    vec3 u, v, w;
    buildOrthonormalBasis(normal, u, v, w);
    vec3 localDir = randomCosineDirection();
    rayOutDir = localDir.x * u + localDir.y * v + localDir.z * w;
    pdf = 0.5f / (float)PI;
}

vec3 DiffuseMaterial::brdf(const vec3 &rayInDir, const vec3 &rayOutDir, const vec3 &normal) const {
    // Actually, the rayOutDir is the rayInDir for real light ray
    if (dot(normal, rayOutDir) > 0.0f) return albedo / (float)PI;
    else return {0, 0, 0};
};

void MicrofacetMaterial::reflect(const vec3 &rayInDir, const vec3 &normal, vec3 &rayOutDir, float &pdf) const {

}

vec3 MicrofacetMaterial::brdf(const vec3 &rayInDir, const vec3 &rayOutDir, const vec3 &normal) const {

}