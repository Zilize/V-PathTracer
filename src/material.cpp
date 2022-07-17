#include "material.h"
#include "utils.h"

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
}

void MirrorMaterial::reflect(const vec3 &rayInDir, const vec3 &normal, vec3 &rayOutDir, float &pdf) const {
    rayOutDir = rayInDir - 2.0f * normal * dot(normal, rayInDir);
    pdf = 1.0f;
}

vec3 MirrorMaterial::brdf(const vec3 &rayInDir, const vec3 &rayOutDir, const vec3 &normal) const {
    if (dot(normal, rayOutDir) > 0.0f) return albedo;
    else return {0, 0, 0};
}

void MicrofacetMaterial::reflect(const vec3 &rayInDir, const vec3 &normal, vec3 &rayOutDir, float &pdf) const {
    throw std::runtime_error("Not implemented.");
}

vec3 MicrofacetMaterial::brdf(const vec3 &rayInDir, const vec3 &rayOutDir, const vec3 &normal) const {
    if (dot(normal, rayOutDir) > 0.0f) return albedo / (float)PI;
    else return {0, 0, 0};
}