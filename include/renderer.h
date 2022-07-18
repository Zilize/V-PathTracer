#ifndef VPATHTRACER_RENDERER_H
#define VPATHTRACER_RENDERER_H

#include "common.h"
#include "scene.h"
#include "camera.h"

class Renderer {
public:
    Renderer() = default;

    void init(Config *_config);
    void clear();
    void render();
    void buildGBuffer();

    void filterByGauss();
    void filterByBilateral();
    void filterByJoint();
    void filterByOutlineRemoval();

    uint8_t *getGBufferDepth();
    uint8_t *getGBufferNormal();
    uint8_t *getGBufferColor();
    uint8_t *getGBufferPosition();
    uint8_t *getFramebuffer();
    uint8_t *getFramebufferAfterFilter();

    void dumpFramebuffer();
    void dumpFramebufferAfterFilter();
    void dumpFile(vector<vec3> &buffer, const string &filename);
    uint8_t *dumpData(vector<vec3> &buffer);

private:
    Config *config;
    Scene *scene;
    Camera *camera;

    vector<float> gBufferDepth;
    vector<vec3> gBufferNormal;
    vector<vec3> gBufferColor;
    vector<vec3> gBufferPosition;
    vector<std::thread> threadPool;
    vector<vector<vec3>> sampleFramebufferPool;  // Framebuffer for each sample, for each thread
    vector<vec3> framebuffer;  // Framebuffer for average shading result until now
    vector<vec3> framebufferAfterFilter;  // Framebuffer after filter utilizing GBuffer

    int currentSampleCount = 0;
};

#endif //VPATHTRACER_RENDERER_H
