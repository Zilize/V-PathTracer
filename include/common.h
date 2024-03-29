#ifndef VPATHTRACER_COMMON_H
#define VPATHTRACER_COMMON_H

#include <iostream>
#include <vector>
#include <stack>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define PI 3.1415926535
#define TWO_PI 6.283185307
#define INV_PI 0.31830988618
#define INV_TWO_PI 0.15915494309

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 512
#define FORM_WIDTH 260
#define FORM_HEIGHT 380
#define VALUE_WIDTH 120
#define MARGIN 16
#define PAD_WIDTH 32
#define PAD_HEIGHT 54
#define APP_HEIGHT (SCREEN_HEIGHT + PAD_HEIGHT > FORM_HEIGHT + 100 ? SCREEN_HEIGHT + PAD_HEIGHT : FORM_HEIGHT + 100)

#define RUSSIAN_ROULETTE 0.8
#define GAUSSIAN_FILTER_SIZE 5
#define GAUSSIAN_FILTER_SIGMA 1.0f
#define BILATERAL_FILTER_SIZE 5
#define BILATERAL_FILTER_DISTANCE_SIGMA 1.0f
#define BILATERAL_FILTER_COLOR_SIGMA 1.0f
#define BILATERAL_FILTER_ACCELERATION
#define JOINT_FILTER_SIZE 3
#define JOINT_FILTER_DEPTH_SIGMA 1e0
#define JOINT_FILTER_NORMAL_SIGMA 1e-2
#define JOINT_FILTER_COLOR_SIGMA 1e1
#define OUTLIER_REMOVAL_FILTER_SIZE 3

#define FILTER_ONLY_LAST_ONE

using std::cout;
using std::endl;
using std::vector;
using std::stack;
using std::string;
using std::shared_ptr;
using std::make_shared;

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;
using glm::radians;
using glm::tan;
using glm::normalize;
using glm::dot;
using glm::cross;
using glm::linearRand;

enum SceneEnum {
    BOX = 0,
    BUNNY
};

enum MaterialEnum {
    DIFFUSE = 0,
    MIRROR,
    MICROFACET
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
    JOINT,
    OUTLIER
};

enum GBufferEnum {
    GBUFFER_NONE = 0,
    DEPTH,
    NORMAL,
    COLOR,
    POSITION
};

typedef struct Config {
    SceneEnum scene;
    int sampleCount;
    MaterialEnum material;
    float roughness;
    int threadCount;
    AccelStructureEnum accelStructure;
    FilterTypeEnum filterType;
    GBufferEnum gBuffer;

    Config(SceneEnum _scene, int _sampleCount, MaterialEnum _material, float _roughness, int _threadCount,
           AccelStructureEnum _accelStructure, FilterTypeEnum _filterType, GBufferEnum _gBuffer):
            scene(_scene), sampleCount(_sampleCount), material(_material), roughness(_roughness),
            threadCount(_threadCount), accelStructure(_accelStructure), filterType(_filterType), gBuffer(_gBuffer) {}
}Config;

#endif //VPATHTRACER_COMMON_H
