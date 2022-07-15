#include "material.h"

bool DiffuseMaterial::reflect(const Ray &rayIn, const HitRecord &hitRecord, Ray &rayOut, float &pdf) const {
    return false;
}

vec3 DiffuseMaterial::brdf(const Ray &rayIn, const Ray &rayOut, const vec3 &normal) const {
    return {0, 0, 0};
};

bool MicrofacetMaterial::reflect(const Ray &rayIn, const HitRecord &hitRecord, Ray &rayOut, float &pdf) const {
    return false;
}

vec3 MicrofacetMaterial::brdf(const Ray &rayIn, const Ray &rayOut, const vec3 &normal) const {

};