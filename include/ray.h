#ifndef VPATHTRACER_RAY_H
#define VPATHTRACER_RAY_H

#include "common.h"

class Ray {
public:
    Ray() = default;
    Ray(const vec3 &_origin, const vec3 &_direction): origin(_origin), direction(_direction) {}

private:
    vec3 origin;
    vec3 direction;
};

#endif //VPATHTRACER_RAY_H
