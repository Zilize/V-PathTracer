#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "application.h"
#include "renderer.h"


Config config(BOX, 16, DIFFUSE, UNIFORM, 0.5, ACCEL_NONE, FILTER_NONE, GBUFFER_NONE);


handleType GLTexture::load(const std::string& fileName) {
    if (mTextureId) {
        glDeleteTextures(1, &mTextureId);
        mTextureId = 0;
    }
    int force_channels = 0;
    int w, h, n;
    handleType textureData(stbi_load(fileName.c_str(), &w, &h, &n, force_channels), stbi_image_free);
    if (!textureData)
        throw std::invalid_argument("Could not load texture data from file " + fileName);
    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    GLint internalFormat;
    GLint format;
    switch (n) {
        case 1: internalFormat = GL_R8; format = GL_RED; break;
        case 2: internalFormat = GL_RG8; format = GL_RG; break;
        case 3: internalFormat = GL_RGB8; format = GL_RGB; break;
        case 4: internalFormat = GL_RGBA8; format = GL_RGBA; break;
        default: internalFormat = 0; format = 0; break;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, textureData.get());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    return textureData;
}

Application::Application() : nanogui::Screen(nanogui::Vector2i(SCREEN_WIDTH + FORM_WIDTH + PAD_WIDTH + MARGIN, SCREEN_HEIGHT + PAD_HEIGHT),
                                "V-PathTracer", false) {
    using namespace nanogui;

    FormHelper *form = new FormHelper(this);
    ref<Window> setting = form->addWindow(Eigen::Vector2i(0, 0), "Settings");
    setting->setFixedWidth(FORM_WIDTH);

    form->addGroup("Rendering Settings");
    auto vScene = form->addVariable("Scene", config.scene);
    vScene->setFixedWidth(VALUE_WIDTH);
    vScene->setItems({"Cornell Box", "Stanford Bunny"});

    auto vSampleNum = form->addVariable("Sample Number", config.sampleNum);
    vSampleNum->setFixedWidth(VALUE_WIDTH);
    vSampleNum->setSpinnable(true);
    vSampleNum->setFormat("[1-9][0-9]*");
    vSampleNum->setMinMaxValues(1, 256);

    auto vMaterial = form->addVariable("Material", config.material, true);
    vMaterial->setFixedWidth(VALUE_WIDTH);
    vMaterial->setItems({"Diffuse", "Microfacet"});

    auto vSampleWay = form->addVariable("Sample Way", config.sampleWay, true);
    vSampleWay->setFixedWidth(VALUE_WIDTH);
    vSampleWay->setItems({"Uniform", "Cosine"});

    auto vRoughness = form->addVariable("Roughness", config.roughness, false);
    vRoughness->setFixedWidth(VALUE_WIDTH);
    vRoughness->setSpinnable(true);
    vRoughness->setMinMaxValues(0.0, 1.0);
    vRoughness->setValueIncrement(0.1);

    form->addGroup("Optimization Settings");
    auto vAccelStructure = form->addVariable("Accel Structure", config.accelStructure, true);
    vAccelStructure->setFixedWidth(VALUE_WIDTH);
    vAccelStructure->setItems({"None", "BVH", "SAH"});

    auto vFilterType = form->addVariable("Filter Type", config.filterType, true);
    vFilterType->setFixedWidth(VALUE_WIDTH);
    vFilterType->setItems({"None", "Gauss", "Bilateral", "Joint"});

    form->addGroup("Show G-Buffer");
    auto vGBuffer = form->addVariable("G-Buffer", config.gBuffer, true);
    vGBuffer->setFixedWidth(VALUE_WIDTH);
    vGBuffer->setItems({"None", "Depth", "Normal", "Color"});

    start = form->addButton("Start", [this]() {
        start->setEnabled(false);
        cout << "======== Render Settings ========" << endl
             << "Scene: " << config.scene << endl
             << "Sample Number: " << config.sampleNum << endl
             << "Material: " << config.material << endl
             << "Sample Way: " << config.sampleWay << endl
             << "Roughness: " << config.roughness << endl
             << "Accel Structure: " << config.accelStructure << endl
             << "Filter Type: " << config.filterType << endl
             << "G-Buffer: " << config.gBuffer << endl;
//        showPicture();
        Renderer renderer(&config, this);
        renderer.run();
    });

    // Rendering Results
    image = new Window(this, "Rendering Result");
    image->setPosition(Vector2i(FORM_WIDTH + MARGIN, 0));
    image->setLayout(new GroupLayout());
    image->setFixedWidth(SCREEN_WIDTH + PAD_WIDTH);
    image->setFixedHeight(SCREEN_HEIGHT + PAD_HEIGHT);

    showPicture();
}

void Application::showPicture() {
    using namespace nanogui;

    if(image->childCount()) image->removeChild(0);

    framebuffers = loadImageDirectory(mNVGContext, "../cache");
    mImageTexture = new GLTexture(framebuffers[0].second);
    mImageTexture->load(framebuffers[0].second + ".png");
    imageView = new ImageView(image, mImageTexture->texture());

    performLayout();
}