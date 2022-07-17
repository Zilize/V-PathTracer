#include "utils.h"

vec3 randomCosineDirection() {
    float r1 = glm::linearRand(0.0f, 1.0f);
    float r2 = glm::linearRand(0.0f, 1.0f);
    auto z = glm::sqrt(1 - r2);
    auto phi = 2 * (float)PI * r1;
    auto x = glm::cos(phi) * glm::sqrt(r2);
    auto y = glm::sin(phi) * glm::sqrt(r2);
    return {x, y, z};
}

void buildOrthonormalBasis(const vec3 &normal, vec3 &u, vec3 &v, vec3 &w) {
    // u, v, w is a right-hand basis
    w = normalize(normal);
    vec3 a = (fabsf(w.x) > 0.9f) ? vec3(0,1,0) : vec3(1,0,0);
    v = normalize(cross(w, a));
    u = normalize(cross(v, w));
}

vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (alpha2 - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

float geometricSmith(float roughness, float NoV, float NoL) {
    float k = (roughness + 1.0f) * (roughness + 1.0f) / 8.0f;
    float gl = NoV / (NoV * (1.0f - k) + k);
    float gv = NoL / (NoL * (1.0f - k) + k);
    return gl * gv;
}