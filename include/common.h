#ifndef VPATHTRACER_COMMON_H
#define VPATHTRACER_COMMON_H

#include <iostream>

#define PI 3.1415926535
#define TWO_PI 6.283185307
#define INV_PI 0.31830988618
#define INV_TWO_PI 0.15915494309

#define SCREEN_WIDTH 784
#define SCREEN_HEIGHT 784
#define FORM_WIDTH 220
#define VALUE_WIDTH 80
#define MARGIN 10
#define PAD_WIDTH 32
#define PAD_HEIGHT 54

using std::cout;
using std::endl;

enum scene_enum {
    BOX = 0,
    BUNNY
};

enum material_enum {
    DIFFUSE = 0,
    MICROFACET
};

enum sample_way_enum {
    UNIFORM = 0,
    COSINE,
};

enum accel_structure_enum {
    ACCEL_NONE = 0,
    BVH,
    SAH
};

enum filter_type_enum {
    FILTER_NONE = 0,
    GAUSS,
    BILATERAL,
    JOINT
};

typedef struct Config {
    scene_enum scene;
    int sampleNum;
    material_enum material;
    sample_way_enum sampleWay;
    float roughness;
    accel_structure_enum accelStructure;
    filter_type_enum filterType;

    Config(scene_enum _scene, int _sampleNum, material_enum _material, sample_way_enum _sampleWay, float _roughness,
           accel_structure_enum _accelStructure, filter_type_enum _filterType):
           scene(_scene), sampleNum(_sampleNum), material(_material), sampleWay(_sampleWay), roughness(_roughness),
           accelStructure(_accelStructure), filterType(_filterType) {}
}Config;

#endif //VPATHTRACER_COMMON_H