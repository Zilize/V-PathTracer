#include "scene.h"

void Scene::loadSceneBox() {
    auto redDiffuseMaterial = make_shared<DiffuseMaterial>(vec3(0.65, 0.05, 0.05));
    auto greenDiffuseMaterial = make_shared<DiffuseMaterial>(vec3(0.12, 0.45, 0.15));
    auto whiteDiffuseMaterial = make_shared<DiffuseMaterial>(vec3(0.73, 0.73, 0.73));
    auto lightMaterial = make_shared<DiffuseLightMaterial>(vec3(15.0, 15.0, 15.0));

    auto floor = new Object("../models/cornellbox/floor.obj", whiteDiffuseMaterial);
    auto left = new Object("../models/cornellbox/left.obj", redDiffuseMaterial);
    auto right = new Object("../models/cornellbox/right.obj", greenDiffuseMaterial);
    auto shortBox = new Object("../models/cornellbox/shortbox.obj", whiteDiffuseMaterial);
    auto tallBox = new Object("../models/cornellbox/tallbox.obj", whiteDiffuseMaterial);
    auto light = new Object("../models/cornellbox/light.obj", lightMaterial);

    objects.emplace_back(floor);
    objects.emplace_back(left);
    objects.emplace_back(right);
    objects.emplace_back(shortBox);
    objects.emplace_back(tallBox);
    objects.emplace_back(light);
}

void Scene::loadSceneBunny() {

}

void Scene::buildNaive() {
    accelStructure = new NaiveAccelStructure(objects);
}

void Scene::buildBVH() {

}

void Scene::buildSAH() {

}

vec3 Scene::castRay(const Ray &ray) {
    return {0, 1, 0};
}

bool Scene::getGBufferInfo(const Ray &ray, float &depth, vec3 &normal, vec3 &color) {
    HitRecord hitRecord;
    bool status = accelStructure->intersect(ray, hitRecord);
    if (!status) return false;

    depth = hitRecord.time;
    normal = hitRecord.normal;
    color = hitRecord.material->getColor();
    return true;
}
