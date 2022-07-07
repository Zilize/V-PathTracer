#ifndef VPATHTRACER_CAMERA_H
#define VPATHTRACER_CAMERA_H

#include "common.h"
#include "ray.h"

class Camera {
public:
    Camera(vec3 lookFrom, vec3 lookAt, vec3 vUp, float fov, float aspectRatio);
    Ray getRayRandom(int row, int col);
    Ray getRayMiddle(int row, int col);
};

#endif //VPATHTRACER_CAMERA_H
