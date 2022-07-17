#ifndef VPATHTRACER_UTILS_H
#define VPATHTRACER_UTILS_H

#include "common.h"

vec3 randomCosineDirection();
void buildOrthonormalBasis(const vec3 &normal, vec3 &u, vec3 &v, vec3 &w);
vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness);
float geometricSmith(float roughness, float NoV, float NoL);
float gaussianFilter(int distance, float sigma);
float bilateralFilter(int distance, vec3 colorDistance, float sigma, float colorSigma);

#endif //VPATHTRACER_UTILS_H
