#ifndef VPATHTRACER_HIT_RECORD_H
#define VPATHTRACER_HIT_RECORD_H

#include "common.h"
class Material;

struct HitRecord {
    vec3 point;  // the hit point on the triangle
    vec3 normal;  // normalized normal
    double time;  // the time ray traverses
    shared_ptr<Material> material;
};

#endif //VPATHTRACER_HIT_RECORD_H
