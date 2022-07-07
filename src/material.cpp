#include "material.h"

bool DiffuseMaterial::reflect(const Ray &rayIn, const HitRecord &hitRecord, Ray &rayOut) const {
    return false;
}

bool MicrofacetMaterial::reflect(const Ray &rayIn, const HitRecord &hitRecord, Ray &rayOut) const {
    return false;
}