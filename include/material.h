#ifndef VPATHTRACER_MATERIAL_H
#define VPATHTRACER_MATERIAL_H

#include "common.h"
#include "ray.h"
#include "triangle.h"

class Material {
public:
    explicit Material(MaterialEnum _type, bool _emissive): type(_type), emissive(_emissive) {}

    MaterialEnum getType() const { return type; }
    bool hasEmission() const { return emissive; }
    virtual vec3 getColor() const = 0;
    virtual void reflect(const vec3 &rayInDir, const vec3 &normal, vec3 &rayOutDir, float &pdf) const = 0;
    virtual vec3 brdf(const vec3 &rayInDir, const vec3 &rayOutDir, const vec3 &normal) const = 0;

private:
    MaterialEnum type;
    bool emissive;
};

class DiffuseMaterial: public Material {
public:
    explicit DiffuseMaterial(const vec3 &_albedo) : Material(DIFFUSE, false), albedo(_albedo) {}
    void reflect(const vec3 &rayInDir, const vec3 &normal, vec3 &rayOutDir, float &pdf) const override;
    vec3 brdf(const vec3 &rayInDir, const vec3 &rayOutDir, const vec3 &normal) const override;

    vec3 getColor() const override { return albedo; }

private:
    vec3 albedo;
};

class MirrorMaterial: public Material {
public:
    explicit MirrorMaterial(const vec3 &_albedo) : Material(MIRROR, false), albedo(_albedo) {}
    void reflect(const vec3 &rayInDir, const vec3 &normal, vec3 &rayOutDir, float &pdf) const override;
    vec3 brdf(const vec3 &rayInDir, const vec3 &rayOutDir, const vec3 &normal) const override;

    vec3 getColor() const override { return albedo; }

private:
    vec3 albedo;
};

class MicrofacetMaterial: public Material {
public:
    MicrofacetMaterial(const vec3 &_albedo, float _roughness): Material(MICROFACET, false), albedo(_albedo), roughness(_roughness) {}
    void reflect(const vec3 &rayInDir, const vec3 &normal, vec3 &rayOutDir, float &pdf) const override;
    vec3 brdf(const vec3 &rayInDir, const vec3 &rayOutDir, const vec3 &normal) const override;

    vec3 getColor() const override { return albedo; }
    float getRoughness() const { return roughness; }

private:
    vec3 albedo;
    float roughness;
};

class DiffuseLightMaterial: public Material {
public:
    explicit DiffuseLightMaterial(const vec3 &_color): Material(DIFFUSE, true), color(_color) {}
    void reflect(const vec3 &rayInDir, const vec3 &normal, vec3 &rayOutDir, float &pdf) const override { return; }
    vec3 brdf(const vec3 &rayInDir, const vec3 &rayOutDir, const vec3 &normal) const override { return {0, 0, 0}; }

    vec3 getColor() const override { return color; }

private:
    vec3 color;
};

#endif //VPATHTRACER_MATERIAL_H
