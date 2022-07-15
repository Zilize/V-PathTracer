#ifndef VPATHTRACER_SCENE_H
#define VPATHTRACER_SCENE_H

#include "common.h"
#include "object.h"
#include "accel_structure.h"

class Scene {
public:
    Scene() = default;

    void loadSceneBox();  // load cornell box
    void loadSceneBunny();  // load stanford bunny
    void add(Object *object) { objects.emplace_back(object); }

    void buildNaive();
    void buildBVH();
    void buildSAH();

    vec3 shade(const HitRecord &hitRecord, const Ray &rayIn);
    vec3 castRay(const Ray &ray);
    bool getGBufferInfo(const Ray &ray, float &depth, vec3 &normal, vec3 &color);

private:
    vector<Object*> objects;
    AccelStructure *accelStructure;
};

#endif //VPATHTRACER_SCENE_H
