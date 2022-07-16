#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "renderer.h"

void Renderer::init(Config *_config) {
    clear();
    config = _config;
    scene = new Scene();

    // Load scene
    if (config->scene == BOX) scene->loadSceneBox();
    else scene->loadSceneBunny();

    // Set camera
    camera = new Camera(vec3(278, 273, -790), vec3(278, 273, 0), vec3(0, 1, 0), 38, 1);

    // Build accelerating structure
    switch (config->accelStructure) {
        case ACCEL_NONE: scene->buildNaive(); break;
        case BVH: scene->buildBVH(); break;
        case SAH: scene->buildSAH(); break;
        default: break;
    }

    if (config->gBuffer != GBUFFER_NONE || (config->filterType != FILTER_NONE && config->filterType != GAUSS)) {
        buildGBuffer();
    }
}

void Renderer::clear() {
    scene = nullptr;

    gBufferDepth.clear();
    gBufferNormal.clear();
    gBufferColor.clear();
    sampleFramebufferPool.clear();
    framebuffer.clear();
    framebufferAfterFilter.clear();
    currentSampleCount = 0;
}

void Renderer::render() {
    sampleFramebuffer.clear();
    for (int i = 0; i < SCREEN_HEIGHT; ++i) {
        for (int j = 0; j < SCREEN_WIDTH; ++j) {
            Ray ray = camera->getRayRandom(i, j);
            vec3 pixel = scene->castRay(ray);
            sampleFramebuffer.emplace_back(pixel);
        }
    }
    if (currentSampleCount == 0) {
        for (auto pixel: sampleFramebuffer)
            framebuffer.emplace_back(pixel);
    }
    else {
        for (int i = 0; i < framebuffer.size(); ++i) {
            framebuffer[i] = (float)currentSampleCount / ((float)currentSampleCount + 1.0f) * framebuffer[i] + 1.0f / ((float)currentSampleCount + 1.0f) * sampleFramebuffer[i];
        }
    }
    currentSampleCount += 1;
    cout << "Sample Counter: " << currentSampleCount << endl;
}

void Renderer::buildGBuffer() {
    for (int i = 0; i < SCREEN_HEIGHT; ++i) {
        for (int j = 0; j < SCREEN_WIDTH; ++j) {
            float depth;
            vec3 normal, color;

            Ray ray = camera->getRayMiddle(i, j);
            if (scene->getGBufferInfo(ray, depth, normal, color)) {
                gBufferDepth.emplace_back(depth);
                gBufferNormal.emplace_back(normal);
                gBufferColor.emplace_back(color);
            }
            else {  // set background color for GBuffer
                gBufferDepth.emplace_back(-1.0);
                gBufferNormal.emplace_back(normalize(vec3(1, 1, 1)));
                gBufferColor.emplace_back(normalize(vec3(1, 1, 1)));
            }
        }
    }
}

void Renderer::filterByGauss() {

}

void Renderer::filterByBilateral() {

}

void Renderer::filterByJoint() {

}

uint8_t *Renderer::getGBufferDepth() {
    float minDepth = std::numeric_limits<float>::max();
    float maxDepth = 0.0f;
    for (auto depth: gBufferDepth) {
        if (depth > maxDepth) maxDepth = depth;
        if (depth < minDepth && depth > 0.0f) minDepth = depth;
    }
    float range = maxDepth - minDepth;
    vector<vec3> gBufferDepthVisual;
    for (auto depth: gBufferDepth) {
        float grayValue = (depth - minDepth) / range;
        gBufferDepthVisual.emplace_back(vec3(grayValue, grayValue, grayValue));
    }
    return dumpData(gBufferDepthVisual);
}

uint8_t *Renderer::getGBufferNormal() {
    vec3 oneVec(1.0, 1.0, 1.0);
    vector<vec3> gBufferNormalVisual;
    for (auto normal: gBufferNormal) {
        gBufferNormalVisual.emplace_back((float)0.5 * (normal + oneVec));
    }
    return dumpData(gBufferNormalVisual);
}

uint8_t *Renderer::getGBufferColor() {
    return dumpData(gBufferColor);
}

uint8_t *Renderer::getFramebuffer() {
    return dumpData(framebuffer);
}

uint8_t *Renderer::getFramebufferAfterFilter() {
    framebufferAfterFilter.clear();
    switch (config->filterType) {
        case GAUSS: filterByGauss(); break;
        case BILATERAL: filterByBilateral(); break;
        case JOINT: filterByJoint(); break;
        default: assert(0); break;
    }
    return dumpData(framebufferAfterFilter);
}

void Renderer::dumpFile(vector<vec3> &buffer) {
    assert(buffer.size() == SCREEN_WIDTH * SCREEN_HEIGHT);
    uint8_t *data = new uint8_t[SCREEN_WIDTH * SCREEN_HEIGHT * 3];

    int index = 0;
    for (auto pixel: buffer) {
        data[index++] = static_cast<uint8_t>(255.999 * glm::clamp(pixel.x, 0.0f, 1.0f));
        data[index++] = static_cast<uint8_t>(255.999 * glm::clamp(pixel.y, 0.0f, 1.0f));
        data[index++] = static_cast<uint8_t>(255.999 * glm::clamp(pixel.z, 0.0f, 1.0f));
    }
    stbi_write_png("../cache/framebuffer.png", SCREEN_WIDTH, SCREEN_HEIGHT, 3, data, SCREEN_WIDTH * 3);
    delete data;
}

uint8_t *Renderer::dumpData(vector<vec3> &buffer) {
    assert(buffer.size() == SCREEN_WIDTH * SCREEN_HEIGHT);
    uint8_t *data = new uint8_t[SCREEN_WIDTH * SCREEN_HEIGHT * 3];

    int index = 0;
    for (auto pixel: buffer) {
        data[index++] = static_cast<uint8_t>(255 * std::powf(glm::clamp(pixel.x, 0.0f, 1.0f), 0.6f));
        data[index++] = static_cast<uint8_t>(255 * std::powf(glm::clamp(pixel.y, 0.0f, 1.0f), 0.6f));
        data[index++] = static_cast<uint8_t>(255 * std::powf(glm::clamp(pixel.z, 0.0f, 1.0f), 0.6f));
    }
    return data;  // Todo: data not delete yet.
}
