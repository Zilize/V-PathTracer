#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <unistd.h>
#include "renderer.h"

void Renderer::run() {
    // Load scene

    // Set camera

    // Build accelerating structure

    // Obtain GBuffer according to different task

    // Rendering: update framebuffer (multi-thread)
    // Dump framebuffer and call showPicture each sample
    for (int i = 0; i < config->sampleNum; ++i) {
        render(0.25 + i * 0.02);
        dump();
        application->showPicture();
    }
}

void Renderer::render(double bbb) {
    // for debug
    framebuffer.clear();
    for (int j = SCREEN_HEIGHT - 1; j >= 0; --j) {
        for (int i = 0; i < SCREEN_WIDTH; ++i) {
            auto r = double(i) / (SCREEN_WIDTH - 1);
            auto g = double(j) / (SCREEN_HEIGHT - 1);
            auto b = bbb;

            framebuffer.emplace_back(vec3(r, g, b));
        }
    }
}

void Renderer::dump() {
    assert(framebuffer.size() == SCREEN_WIDTH * SCREEN_HEIGHT);
    uint8_t *data = new uint8_t[SCREEN_WIDTH * SCREEN_HEIGHT * 3];

    int index = 0;
    for (auto pixel: framebuffer) {
        data[index++] = static_cast<uint8_t>(255.999 * pixel.x);
        data[index++] = static_cast<uint8_t>(255.999 * pixel.y);
        data[index++] = static_cast<uint8_t>(255.999 * pixel.z);
    }
    stbi_write_png("../cache/framebuffer.png", SCREEN_WIDTH, SCREEN_HEIGHT, 3, data, SCREEN_WIDTH * 3);
}