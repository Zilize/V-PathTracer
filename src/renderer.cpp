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

uint8_t *Renderer::render() {
    // for debug
    framebuffer.clear();
    for (int j = SCREEN_HEIGHT - 1; j >= 0; --j) {
        for (int i = 0; i < SCREEN_WIDTH; ++i) {
            auto r = double(i) / (SCREEN_WIDTH - 1);
            auto g = double(j) / (SCREEN_HEIGHT - 1);
            auto b = 0.25;

            framebuffer.emplace_back(vec3(r, g, b));
        }
    }

    return dumpData();
}

void Renderer::dumpFile() {
    assert(framebuffer.size() == SCREEN_WIDTH * SCREEN_HEIGHT);
    uint8_t *data = new uint8_t[SCREEN_WIDTH * SCREEN_HEIGHT * 3];

    int index = 0;
    for (auto pixel: framebuffer) {
        data[index++] = static_cast<uint8_t>(255.999 * pixel.x);
        data[index++] = static_cast<uint8_t>(255.999 * pixel.y);
        data[index++] = static_cast<uint8_t>(255.999 * pixel.z);
    }
    stbi_write_png("../cache/framebuffer.png", SCREEN_WIDTH, SCREEN_HEIGHT, 3, data, SCREEN_WIDTH * 3);
    delete data;
}

uint8_t *Renderer::dumpData() {
    assert(framebuffer.size() == SCREEN_WIDTH * SCREEN_HEIGHT);
    uint8_t *data = new uint8_t[SCREEN_WIDTH * SCREEN_HEIGHT * 3];

    int index = 0;
    for (auto pixel: framebuffer) {
        data[index++] = static_cast<uint8_t>(255.999 * pixel.x);
        data[index++] = static_cast<uint8_t>(255.999 * pixel.y);
        data[index++] = static_cast<uint8_t>(255.999 * pixel.z);
    }
    return data;  // Todo: data not delete yet.
}