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
    auto redDiffuseMaterial = make_shared<DiffuseMaterial>(vec3(0.65, 0.05, 0.05));
    auto greenDiffuseMaterial = make_shared<DiffuseMaterial>(vec3(0.12, 0.45, 0.15));
    auto whiteDiffuseMaterial = make_shared<DiffuseMaterial>(vec3(0.73, 0.73, 0.73));
    auto lightMaterial = make_shared<DiffuseLightMaterial>(vec3(15.0, 15.0, 15.0));

    auto floor = new Object("../models/cornellbox/floor.obj", whiteDiffuseMaterial);
    auto left = new Object("../models/cornellbox/left.obj", redDiffuseMaterial);
    auto right = new Object("../models/cornellbox/right.obj", greenDiffuseMaterial);
    auto light = new Object("../models/cornellbox/light.obj", lightMaterial);
    auto bunny = new Object("../models/bunny/bunny.obj", whiteDiffuseMaterial);

    mat4 rotate = mat4(1.0f);
    rotate = glm::rotate(rotate, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));

    // Rotate
    for (auto &triangle: bunny->primitives) {
        vec4 v0_4 = rotate * vec4(triangle.v0, 1.0f);
        triangle.v0 = vec3(v0_4.x, v0_4.y, v0_4.z);
        vec4 v1_4 = rotate * vec4(triangle.v1, 1.0f);
        triangle.v1 = vec3(v1_4.x, v1_4.y, v1_4.z);
        vec4 v2_4 = rotate * vec4(triangle.v2, 1.0f);
        triangle.v2 = vec3(v2_4.x, v2_4.y, v2_4.z);

        vec4 n0_4 = rotate * vec4(triangle.n0, 1.0f);
        triangle.n0 = vec3(n0_4.x, n0_4.y, n0_4.z);
        vec4 n1_4 = rotate * vec4(triangle.n1, 1.0f);
        triangle.n1 = vec3(n1_4.x, n1_4.y, n1_4.z);
        vec4 n2_4 = rotate * vec4(triangle.n2, 1.0f);
        triangle.n2 = vec3(n2_4.x, n2_4.y, n2_4.z);
    }

    // Scale and translation
    float scale = 1600.0f;
    float transX = 250.0f, transY = -54.0f, transZ = 220.0f;
    for (auto &triangle: bunny->primitives) {
        triangle.v0.x *= scale;
        triangle.v0.y *= scale;
        triangle.v0.z *= scale;
        triangle.v1.x *= scale;
        triangle.v1.y *= scale;
        triangle.v1.z *= scale;
        triangle.v2.x *= scale;
        triangle.v2.y *= scale;
        triangle.v2.z *= scale;

        triangle.v0.x += transX;
        triangle.v1.x += transX;
        triangle.v2.x += transX;
        triangle.v0.y += transY;
        triangle.v1.y += transY;
        triangle.v2.y += transY;
        triangle.v0.z += transZ;
        triangle.v1.z += transZ;
        triangle.v2.z += transZ;
    }

    objects.emplace_back(floor);
    objects.emplace_back(left);
    objects.emplace_back(right);
    objects.emplace_back(light);
    objects.emplace_back(bunny);
}

void Scene::buildNaive() {
    accelStructure = new NaiveAccelStructure(objects);
}

void Scene::buildBVH() {
    accelStructure = new BVHAccelStructure(objects);
}

void Scene::buildSAH() {
    accelStructure = new SAHAccelStructure(objects);
}

vec3 Scene::shade(const HitRecord &hitRecord, const Ray &rayIn) {
    return {0, 0, 0};
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
