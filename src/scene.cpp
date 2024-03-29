#include "scene.h"
#include "utils.h"

void Scene::loadSceneBox(Config *config) {
    auto redDiffuseMaterial = make_shared<DiffuseMaterial>(vec3(0.63, 0.065, 0.05));
    auto greenDiffuseMaterial = make_shared<DiffuseMaterial>(vec3(0.14, 0.45, 0.091));
    auto whiteDiffuseMaterial = make_shared<DiffuseMaterial>(vec3(0.725, 0.71, 0.68));
    auto lightMaterial = make_shared<DiffuseLightMaterial>(vec3(23.918, 19.283, 15.541));

    shared_ptr<Material> tallBoxMaterial;
    switch (config->material) {
        case DIFFUSE: tallBoxMaterial = whiteDiffuseMaterial; break;
        case MIRROR: tallBoxMaterial = make_shared<MirrorMaterial>(vec3(0.725, 0.71, 0.68)); break;
        case MICROFACET: tallBoxMaterial = make_shared<MicrofacetMaterial>(vec3(0.725, 0.71, 0.68), config->roughness); break;
        default: throw std::runtime_error("Material not supported.");
    }

    auto floor = new Object("../models/cornellbox/floor.obj", whiteDiffuseMaterial);
    auto left = new Object("../models/cornellbox/left.obj", redDiffuseMaterial);
    auto right = new Object("../models/cornellbox/right.obj", greenDiffuseMaterial);
    auto shortBox = new Object("../models/cornellbox/shortbox.obj", whiteDiffuseMaterial);
    auto tallBox = new Object("../models/cornellbox/tallbox.obj", tallBoxMaterial);
    auto light = new Object("../models/cornellbox/light.obj", lightMaterial);

    objects.emplace_back(floor);
    objects.emplace_back(left);
    objects.emplace_back(right);
    objects.emplace_back(shortBox);
    objects.emplace_back(tallBox);
    objects.emplace_back(light);

    for (auto object: objects) {
        for (auto &primitive: object->primitives) {
            primitive.n0 = -primitive.n0;
            primitive.n1 = -primitive.n1;
            primitive.n2 = -primitive.n2;
        }
    }
}

void Scene::loadSceneBunny(Config *config) {
    auto redDiffuseMaterial = make_shared<DiffuseMaterial>(vec3(0.63, 0.065, 0.05));
    auto greenDiffuseMaterial = make_shared<DiffuseMaterial>(vec3(0.14, 0.45, 0.091));
    auto whiteDiffuseMaterial = make_shared<DiffuseMaterial>(vec3(0.725, 0.71, 0.68));
    auto lightMaterial = make_shared<DiffuseLightMaterial>(vec3(23.918, 19.283, 15.541));

    shared_ptr<Material> bunnyMaterial;
    switch (config->material) {
        case DIFFUSE: bunnyMaterial = whiteDiffuseMaterial; break;
        case MIRROR: bunnyMaterial = make_shared<MirrorMaterial>(vec3(0.725, 0.71, 0.68)); break;
        case MICROFACET: bunnyMaterial = make_shared<MicrofacetMaterial>(vec3(0.725, 0.71, 0.68), config->roughness); break;
        default: throw std::runtime_error("Material not supported.");
    }

    auto floor = new Object("../models/cornellbox/floor.obj", whiteDiffuseMaterial);
    auto left = new Object("../models/cornellbox/left.obj", redDiffuseMaterial);
    auto right = new Object("../models/cornellbox/right.obj", greenDiffuseMaterial);
    auto light = new Object("../models/cornellbox/light.obj", lightMaterial);
    auto bunny = new Object("../models/bunny/bunny.obj", bunnyMaterial);

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

    for (auto object: objects) {
        for (auto &primitive: object->primitives) {
            primitive.n0 = -primitive.n0;
            primitive.n1 = -primitive.n1;
            primitive.n2 = -primitive.n2;
        }
    }
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

void Scene::sampleLight(vec3 &point, vec3 &normal, vec3 &color, float &pdf) {
    float lightAreaSum = 0.0f;
    for (auto & object : objects) {
        for (auto & primitive : object->primitives) {
            if (primitive.material->hasEmission()) {
                lightAreaSum += primitive.getArea();
            }
        }
    }
    float sampleArea = glm::linearRand(0.0f, 1.0f) * lightAreaSum;
    float lightAreaPointer = 0.0f;
    for (auto & object : objects) {
        for (auto & primitive : object->primitives) {
            if (primitive.material->hasEmission()) {
                lightAreaPointer += primitive.getArea();
                if (lightAreaPointer > sampleArea) {
                    primitive.sample(point, normal);
                    color = primitive.material->getColor();
                    pdf = 1.0f / lightAreaSum;
                    return;
                }
            }
        }
    }
}

vec3 Scene::shadeDiffuse(const HitRecord &hitRecord, const Ray &rayIn) {
    // Contribution from the light source
    vec3 lightPoint, lightNormal, lightColor;
    float lightPdf;
    sampleLight(lightPoint, lightNormal, lightColor, lightPdf);
    vec3 hitToLight = lightPoint - hitRecord.point;
    vec3 hitToLightUnit = normalize(hitToLight);

    vec3 lightDir = {0, 0, 0};
    HitRecord lightTestHitRecord;  // to test if hit point can see light
    bool lightTestStatus = accelStructure->intersect(Ray(hitRecord.point, hitToLightUnit), lightTestHitRecord);
    auto lightBias = length(lightTestHitRecord.point - lightPoint);
    if (lightTestStatus && lightBias < 0.0005f) {
        vec3 brdf = hitRecord.material->brdf(rayIn.direction, hitToLightUnit, hitRecord.normal);
        float cosTheta = std::max(0.0f, dot(hitRecord.normal, hitToLightUnit));
        float cosThetaPi = std::max(0.0f, dot(lightNormal, -hitToLightUnit));
        lightDir += lightColor * brdf * cosTheta * cosThetaPi / dot(hitToLight, hitToLight) / lightPdf;
    }

    // Contribution from other reflectors
    vec3 lightInDir = {0, 0, 0};
    if (glm::linearRand(0.0f, 1.0f) < RUSSIAN_ROULETTE) {
        vec3 rayOutDir;
        float pdf;
        hitRecord.material->reflect(rayIn.direction, hitRecord.normal, rayOutDir, pdf);

        HitRecord reflectorHitRecord;
        bool reflectorStatus = accelStructure->intersect(Ray(hitRecord.point, rayOutDir), reflectorHitRecord);
        if (reflectorStatus && !reflectorHitRecord.material->hasEmission()) {
            vec3 brdf = hitRecord.material->brdf(rayIn.direction, rayOutDir, hitRecord.normal);
            float cosTheta = std::max(0.0f, dot(hitRecord.normal, rayOutDir));
            lightInDir += shade(reflectorHitRecord, Ray(hitRecord.point, rayOutDir)) * brdf * cosTheta / pdf / (float)RUSSIAN_ROULETTE;
        }
    }

    return lightDir + lightInDir;
}

vec3 Scene::shadeMirror(const HitRecord &hitRecord, const Ray &rayIn) {
    // Contribution from other reflectors
    vec3 lightInDir = {0, 0, 0};
    if (glm::linearRand(0.0f, 1.0f) < RUSSIAN_ROULETTE) {
        vec3 rayOutDir;
        float pdf;
        hitRecord.material->reflect(rayIn.direction, hitRecord.normal, rayOutDir, pdf);

        HitRecord reflectorHitRecord;
        bool reflectorStatus = accelStructure->intersect(Ray(hitRecord.point, rayOutDir), reflectorHitRecord);
        if (reflectorStatus) {
            lightInDir += shade(reflectorHitRecord, Ray(hitRecord.point, rayOutDir)) / (float)RUSSIAN_ROULETTE;
        }
    }

    return lightInDir;
}

vec3 Scene::shadeMicrofacet(const HitRecord &hitRecord, const Ray &rayIn) {
    // Contribution from the light source
    vec3 lightPoint, lightNormal, lightColor;
    float lightPdf;
    sampleLight(lightPoint, lightNormal, lightColor, lightPdf);
    vec3 hitToLight = lightPoint - hitRecord.point;
    vec3 hitToLightUnit = normalize(hitToLight);

    vec3 lightDir = {0, 0, 0};
    HitRecord lightTestHitRecord;  // to test if hit point can see light
    bool lightTestStatus = accelStructure->intersect(Ray(hitRecord.point, hitToLightUnit), lightTestHitRecord);
    auto lightBias = length(lightTestHitRecord.point - lightPoint);
    if (lightTestStatus && lightBias < 0.0005f) {
        vec3 brdf = hitRecord.material->brdf(rayIn.direction, hitToLightUnit, hitRecord.normal);
        float cosTheta = std::max(0.0f, dot(hitRecord.normal, hitToLightUnit));
        float cosThetaPi = std::max(0.0f, dot(lightNormal, -hitToLightUnit));
        lightDir += lightColor * brdf * cosTheta * cosThetaPi / dot(hitToLight, hitToLight) / lightPdf;
    }

    // Contribution from other reflectors
    vec3 lightInDir = {0, 0, 0};
    if (glm::linearRand(0.0f, 1.0f) < RUSSIAN_ROULETTE) {
        vec2 Xi = glm::linearRand(vec2(0.0f, 0.0f), vec2(1.0f, 1.0f));
        auto microfacetMaterial = std::static_pointer_cast<MicrofacetMaterial>(hitRecord.material);
        vec3 H = importanceSampleGGX(Xi, hitRecord.normal, microfacetMaterial->getRoughness());
        vec3 V = -rayIn.direction;
        vec3 L = 2.0f * dot(V, H) * H - V;

        float NoL = glm::clamp(dot(hitRecord.normal, L), 0.0f, 1.0f);
        if (NoL > 0.0f) {
            HitRecord reflectorHitRecord;
            bool reflectorStatus = accelStructure->intersect(Ray(hitRecord.point, L), reflectorHitRecord);
            if (reflectorStatus && !reflectorHitRecord.material->hasEmission()) {
                float NoV = glm::clamp(dot(hitRecord.normal, V), 0.0f, 1.0f);
                float NoH = glm::clamp(dot(hitRecord.normal, H), 0.0f, 1.0f);
                float VoH = glm::clamp(dot(V, H), 0.0f, 1.0f);

                if (NoL > 0.0f) {
                    float G = geometricSmith(microfacetMaterial->getRoughness(), NoV, NoL);
                    float Fc = std::pow(1.0f - VoH, 5.0f);
                    vec3 F = (1.0f - Fc) * microfacetMaterial->getColor() + vec3(Fc, Fc, Fc);
                    lightInDir += shade(reflectorHitRecord, Ray(hitRecord.point, L)) * F * G * VoH / (NoH * NoV) / (float)RUSSIAN_ROULETTE;
                }
            }
        }
    }

    return lightDir + lightInDir;
}

inline vec3 Scene::shade(const HitRecord &hitRecord, const Ray &rayIn) {
    if (hitRecord.material->hasEmission()) return hitRecord.material->getColor();
    switch (hitRecord.material->getType()) {
        case DIFFUSE: return shadeDiffuse(hitRecord, rayIn);
        case MIRROR: return shadeMirror(hitRecord, rayIn);
        case MICROFACET: return shadeMicrofacet(hitRecord, rayIn);
        default: throw std::runtime_error("Material not supported.");
    }
}

vec3 Scene::castRay(const Ray &ray) {
    HitRecord hitRecord;
    bool status = accelStructure->intersect(ray, hitRecord);
    if (!status) return {0, 0, 0};  // background color black
    else return shade(hitRecord, ray);
}

bool Scene::getGBufferInfo(const Ray &ray, float &depth, vec3 &normal, vec3 &color, vec3 &position) {
    HitRecord hitRecord;
    bool status = accelStructure->intersect(ray, hitRecord);
    if (!status) return false;

    depth = hitRecord.time;
    normal = hitRecord.normal;
    color = hitRecord.material->getColor();
    position = hitRecord.point;
    return true;
}
