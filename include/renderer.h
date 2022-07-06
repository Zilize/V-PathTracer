#ifndef VPATHTRACER_RENDERER_H
#define VPATHTRACER_RENDERER_H

#include "common.h"

class Renderer {
public:
    Renderer() = default;

    void init(Config *_config);
    void render(float blue);
    void filterByGauss();
    void filterByBilateral();
    void filterByJoint();

    uint8_t *getGBufferDepth();
    uint8_t *getGBufferNormal();
    uint8_t *getGBufferColor();
    uint8_t *getFramebuffer();
    uint8_t *getFramebufferAfterFilter();

    void dumpFile(vector<vec3> &buffer);
    uint8_t *dumpData(vector<vec3> &buffer);

private:
    Config *config;

    vector<float> gBufferDepth;
    vector<vec3> gBufferNormal;
    vector<vec3> gBufferColor;
    vector<vector<vec3>> sampleFramebufferPool;  // Framebuffer for each sample, for each thread
    vector<vec3> framebuffer;  // Framebuffer for average shading result until now
    vector<vec3> framebufferAfterFilter;  // Framebuffer after filter utilizing GBuffer

    int currentSampleCount = 0;
};

#endif //VPATHTRACER_RENDERER_H
