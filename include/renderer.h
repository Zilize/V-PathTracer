#ifndef VPATHTRACER_RENDERER_H
#define VPATHTRACER_RENDERER_H

#include "common.h"

class Renderer {
public:
    Renderer() = default;
    Renderer(Config *_config) {
        config = _config;
    }

private:
    Config *config;
};

#endif //VPATHTRACER_RENDERER_H
