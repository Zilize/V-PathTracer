#ifndef VPATHTRACER_COMMON_H
#define VPATHTRACER_COMMON_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define PI 3.1415926535
#define TWO_PI 6.283185307
#define INV_PI 0.31830988618
#define INV_TWO_PI 0.15915494309

#define SCREEN_WIDTH 784
#define SCREEN_HEIGHT 784
#define FORM_WIDTH 260
#define VALUE_WIDTH 120
#define MARGIN 10
#define PAD_WIDTH 32
#define PAD_HEIGHT 54

using std::cout;
using std::endl;
using std::vector;

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

enum SceneEnum {
    BOX = 0,
    BUNNY
};

enum MaterialEnum {
    DIFFUSE = 0,
    MICROFACET
};

enum SampleWayEnum {
    UNIFORM = 0,
    COSINE,
};

enum AccelStructureEnum {
    ACCEL_NONE = 0,
    BVH,
    SAH
};

enum FilterTypeEnum {
    FILTER_NONE = 0,
    GAUSS,
    BILATERAL,
    JOINT
};

enum GBufferEnum {
    GBUFFER_NONE = 0,
    DEPTH,
    NORMAL,
    COLOR
};

typedef struct Config {
    SceneEnum scene;
    int sampleCount;
    MaterialEnum material;
    SampleWayEnum sampleWay;
    float roughness;
    int threadCount;
    AccelStructureEnum accelStructure;
    FilterTypeEnum filterType;
    GBufferEnum gBuffer;

    Config(SceneEnum _scene, int _sampleCount, MaterialEnum _material, SampleWayEnum _sampleWay, float _roughness,
           int _threadCount, AccelStructureEnum _accelStructure, FilterTypeEnum _filterType, GBufferEnum _gBuffer):
            scene(_scene), sampleCount(_sampleCount), material(_material), sampleWay(_sampleWay), roughness(_roughness),
            threadCount(_threadCount), accelStructure(_accelStructure), filterType(_filterType), gBuffer(_gBuffer) {}
}Config;

#endif //VPATHTRACER_COMMON_H