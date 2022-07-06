#ifndef VPATHTRACER_RENDERER_H
#define VPATHTRACER_RENDERER_H

#include "common.h"

class Renderer {
public:
    Renderer() = default;

    void init(Config *_config);
    uint8_t *render();
    void dumpFile();
    uint8_t *dumpData();

private:
    Config *config;
    vector<vec3> framebuffer;
};

#endif //VPATHTRACER_RENDERER_H
