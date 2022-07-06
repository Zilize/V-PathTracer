#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <unistd.h>
#include "renderer.h"

void Renderer::init(Config *_config) {
    if (!config) {
        // TODO: clear all the buffer
        config = _config;
    }

    // Load scene

    // Set camera

    // Build accelerating structure

    // Obtain GBuffer according to different task

}

void Renderer::render(float blue) {
    // for debug
    framebuffer.clear();
    for (int j = SCREEN_HEIGHT - 1; j >= 0; --j) {
        for (int i = 0; i < SCREEN_WIDTH; ++i) {
            auto r = double(i) / (SCREEN_WIDTH - 1);
            auto g = double(j) / (SCREEN_HEIGHT - 1);
            auto b = blue;

            framebuffer.emplace_back(vec3(r, g, b));
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
    float minDepth = gBufferDepth[0];
    float maxDepth = gBufferDepth[0];
    for (auto depth: gBufferDepth) {
        if (depth > maxDepth) maxDepth = depth;
        if (depth < minDepth) minDepth = depth;
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
        data[index++] = static_cast<uint8_t>(255.999 * pixel.x);
        data[index++] = static_cast<uint8_t>(255.999 * pixel.y);
        data[index++] = static_cast<uint8_t>(255.999 * pixel.z);
    }
    stbi_write_png("../cache/framebuffer.png", SCREEN_WIDTH, SCREEN_HEIGHT, 3, data, SCREEN_WIDTH * 3);
    delete data;
}

uint8_t *Renderer::dumpData(vector<vec3> &buffer) {
    assert(buffer.size() == SCREEN_WIDTH * SCREEN_HEIGHT);
    uint8_t *data = new uint8_t[SCREEN_WIDTH * SCREEN_HEIGHT * 3];

    int index = 0;
    for (auto pixel: buffer) {
        data[index++] = static_cast<uint8_t>(255.999 * pixel.x);
        data[index++] = static_cast<uint8_t>(255.999 * pixel.y);
        data[index++] = static_cast<uint8_t>(255.999 * pixel.z);
    }
    return data;  // Todo: data not delete yet.
}