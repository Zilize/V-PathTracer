#ifndef VPATHTRACER_MATERIAL_H
#define VPATHTRACER_MATERIAL_H

#include "common.h"
#include "ray.h"
#include "triangle.h"

class Material {
public:
    explicit Material(const vec3 &_albedo): albedo(_albedo) {}

    vec3 getColor() { return albedo; }
    virtual bool reflect(const Ray &rayIn, const HitRecord &hitRecord, Ray &rayOut) const = 0;

private:
    vec3 albedo;
};

class DiffuseMaterial: public Material {
public:
    explicit DiffuseMaterial(const vec3 &_albedo) : Material(_albedo) {}
    bool reflect(const Ray &rayIn, const HitRecord &hitRecord, Ray &rayOut) const override;
};

class MicrofacetMaterial: public Material {
public:
    MicrofacetMaterial(const vec3 &_albedo, float _roughness): Material(_albedo), roughness(_roughness) {}
    bool reflect(const Ray &rayIn, const HitRecord &hitRecord, Ray &rayOut) const override;

private:
    float roughness;
};

class DiffuseLightMaterial: public Material {
public:
    explicit DiffuseLightMaterial(const vec3 &_albedo): Material(_albedo) {}
    bool reflect(const Ray &rayIn, const HitRecord &hitRecord, Ray &rayOut) const override { return false; }
};

#endif //VPATHTRACER_MATERIAL_H
