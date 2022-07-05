#ifndef VPATHTRACER_RENDERER_H
#define VPATHTRACER_RENDERER_H

#include "common.h"
#include "application.h"

class Renderer {
public:
    Renderer() = default;
    Renderer(Config *_config, Application *_application) {
        config = _config;
        application = _application;
    }

    void run();
    void render();
    void dumpFile();
    uint8_t *dumpData();

private:
    Config *config;
    Application *application;
    vector<vec3> framebuffer;
};

#endif //VPATHTRACER_RENDERER_H
