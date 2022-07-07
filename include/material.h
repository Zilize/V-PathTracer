#ifndef VPATHTRACER_MATERIAL_H
#define VPATHTRACER_MATERIAL_H

#include "common.h"
#include "ray.h"
#include "triangle.h"

class Material {
public:
    Material() = default;
    virtual bool reflect(const Ray &rayIn, const HitRecord &hitRecord, Ray &rayOut) const = 0;
};

class DiffuseMaterial: public Material {
public:
    explicit DiffuseMaterial(const vec3 &_albedo): albedo(_albedo) {}
    bool reflect(const Ray &rayIn, const HitRecord &hitRecord, Ray &rayOut) const override;

private:
    vec3 albedo;
};

class MicrofacetMaterial: public Material {
public:
    MicrofacetMaterial(const vec3 &_albedo, double _roughness): albedo(_albedo), roughness(_roughness) {}
    bool reflect(const Ray &rayIn, const HitRecord &hitRecord, Ray &rayOut) const override;

private:
    vec3 albedo;
    double roughness;
};

class DiffuseLightMaterial: public Material {
public:
    explicit DiffuseLightMaterial(const vec3 &_color): color(_color) {}
    bool reflect(const Ray &rayIn, const HitRecord &hitRecord, Ray &rayOut) const override { return false; }

private:
    vec3 color;
};

#endif //VPATHTRACER_MATERIAL_H
