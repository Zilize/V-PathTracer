#ifndef VPATHTRACER_SCENE_H
#define VPATHTRACER_SCENE_H

#include "common.h"
#include "object.h"

class Scene {
public:
    Scene() = default;

    void loadSceneBox();  // load cornell box
    void loadSceneBunny();  // load stanford bunny
    void add(Object *object) { objects.emplace_back(object); }

    void buildBVH();
    void buildSAH();

    void intersect(const Ray &ray, HitRecord &hitRecord);

private:
    vector<Object*> objects;
};

#endif //VPATHTRACER_SCENE_H
