#include "camera.h"

Camera::Camera(vec3 lookFrom, vec3 lookAt, vec3 vUp, float fov, float aspectRatio) {
    auto theta = radians(fov);
    auto h = tan(theta / 2.0);
    auto viewportHeight = 2.0 * h;
    auto viewportWidth = aspectRatio * viewportHeight;

    auto w = normalize(lookFrom - lookAt);
    auto u = normalize(cross(vUp, w));
    auto v = - cross(w, u);

    origin = lookFrom;
    horizontal = (float)viewportWidth * u;
    vertical = (float)viewportHeight * v;
    upperLeftCorner = origin - w - horizontal / 2.0f - vertical / 2.0f;
}

Ray Camera::getRayRandom(int row, int col) {
    auto sample = linearRand(vec2(0.0), vec2(1.0));
    auto rowScale = (float)row + sample.x;
    auto colScale = (float)col + sample.y;
    auto lookAt = upperLeftCorner + vertical * rowScale / (float)SCREEN_HEIGHT + horizontal * colScale / (float)SCREEN_WIDTH;
    return Ray(origin, lookAt - origin);
}

Ray Camera::getRayMiddle(int row, int col) {
    auto rowScale = (float)row + 0.5f;
    auto colScale = (float)col + 0.5f;
    auto lookAt = upperLeftCorner + vertical * rowScale / (float)SCREEN_HEIGHT + horizontal * colScale / (float)SCREEN_WIDTH;
    return Ray(origin, lookAt - origin);
}