#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <thread>
#include "application.h"
#include "renderer.h"


handleType GLTexture::loadFile(const std::string& fileName) {
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
    GLint internalFormat = GL_RGB8;
    GLint format = GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, textureData.get());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    return textureData;
}

handleType GLTexture::loadData(uint8_t *data, int w, int h) {
    if (mTextureId) {
        glDeleteTextures(1, &mTextureId);
        mTextureId = 0;
    }
    handleType textureData(data, stbi_image_free);
    if (!textureData)
        throw std::invalid_argument("Could not load texture from data");
    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    GLint internalFormat = GL_RGB8;
    GLint format = GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, textureData.get());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    return textureData;
}

Application::Application() : nanogui::Screen(nanogui::Vector2i(SCREEN_WIDTH + FORM_WIDTH + PAD_WIDTH + 3 * MARGIN, APP_HEIGHT + 2 * MARGIN),
                                "V-PathTracer", false) {
    using namespace nanogui;

    renderer = new Renderer();
    config = new Config(BOX, 1, DIFFUSE, UNIFORM, 0.5, 1, ACCEL_NONE, FILTER_NONE, GBUFFER_NONE);

    FormHelper *form = new FormHelper(this);
    ref<Window> setting = form->addWindow(Eigen::Vector2i(MARGIN, MARGIN), "Settings");
    setting->setFixedWidth(FORM_WIDTH);
    setting->setFixedHeight(FORM_HEIGHT);

    form->addGroup("Rendering Settings");
    auto vScene = form->addVariable("Scene", config->scene);
    vScene->setFixedWidth(VALUE_WIDTH);
    vScene->setItems({"Cornell Box", "Stanford Bunny"});

    auto vSampleCount = form->addVariable("Sample Count", config->sampleCount);
    vSampleCount->setFixedWidth(VALUE_WIDTH);
    vSampleCount->setSpinnable(true);
    vSampleCount->setFormat("[1-9][0-9]*");
    vSampleCount->setMinMaxValues(1, 256);

    auto vMaterial = form->addVariable("Material", config->material, true);
    vMaterial->setFixedWidth(VALUE_WIDTH);
    vMaterial->setItems({"Diffuse", "Microfacet"});
    vMaterial->setTooltip("If set to Diffuse, the Roughness will be ignored; if set to Microfacet, the Sample Way will be ignored.");

    auto vSampleWay = form->addVariable("Sample Way", config->sampleWay, true);
    vSampleWay->setFixedWidth(VALUE_WIDTH);
    vSampleWay->setItems({"Uniform", "Cosine"});
    vSampleWay->setTooltip("How to sample a direction for diffuse material.");

    auto vRoughness = form->addVariable("Roughness", config->roughness, false);
    vRoughness->setFixedWidth(VALUE_WIDTH);
    vRoughness->setSpinnable(true);
    vRoughness->setMinMaxValues(0.0, 1.0);
    vRoughness->setValueIncrement(0.1);
    vRoughness->setTooltip("Roughness value for microfacet material.");

    form->addGroup("Optimization Settings");
    auto vThreadCount = form->addVariable("Thread Count", config->threadCount, true);
    vThreadCount->setFixedWidth(VALUE_WIDTH);
    vThreadCount->setSpinnable(true);
    vThreadCount->setFormat("[1-9][0-9]*");
    vThreadCount->setMinMaxValues(1, 32);
    vThreadCount->setValueIncrement(1);

    auto vAccelStructure = form->addVariable("Accel Structure", config->accelStructure, true);
    vAccelStructure->setFixedWidth(VALUE_WIDTH);
    vAccelStructure->setItems({"None", "BVH", "SAH"});

    auto vFilterType = form->addVariable("Filter Type", config->filterType, true);
    vFilterType->setFixedWidth(VALUE_WIDTH);
    vFilterType->setItems({"None", "Gauss", "Bilateral", "Joint"});

    form->addGroup("Show GBuffer");
    auto vGBuffer = form->addVariable("GBuffer", config->gBuffer, true);
    vGBuffer->setFixedWidth(VALUE_WIDTH);
    vGBuffer->setItems({"None", "Depth", "Normal", "Color"});
    vGBuffer->setTooltip("If set show GBuffer, the renderer will not perform Monte Carlo rendering.");

    start = form->addButton("Start", [this]() {
        this->run();
    });

    auto progress = new Window(this, "Progress");
    progress->setPosition(Vector2i(MARGIN, FORM_HEIGHT + 2 * MARGIN));
    progress->setLayout(new GroupLayout());
    progress->setFixedWidth(FORM_WIDTH);

    progressBar = new ProgressBar(progress);
    progressBar->setValue(imageDataProgress);

    // Rendering Results
    image = new Window(this, "Rendering Result");
    image->setPosition(Vector2i(FORM_WIDTH + 2 * MARGIN, MARGIN));
    image->setLayout(new GroupLayout());
    image->setFixedWidth(SCREEN_WIDTH + PAD_WIDTH);
    image->setFixedHeight(SCREEN_HEIGHT + PAD_HEIGHT);

    showInit();
}

void Application::run() {
    std::thread threadRun([this]() {
        this->start->setEnabled(false);
        cout << "======== Render Settings ========" << endl
             << "Scene: " << this->config->scene << endl
             << "Sample Count: " << this->config->sampleCount << endl
             << "Material: " << this->config->material << endl
             << "Sample Way: " << this->config->sampleWay << endl
             << "Roughness: " << this->config->roughness << endl
             << "Thread Count: " << this->config->threadCount << endl
             << "Accel Structure: " << this->config->accelStructure << endl
             << "Filter Type: " << this->config->filterType << endl
             << "GBuffer: " << this->config->gBuffer << endl;
        this->renderer->init(this->config);

        // Task Scheduler

        if (this->config->gBuffer != GBUFFER_NONE) {
            uint8_t *data = nullptr;
            switch (this->config->gBuffer) {
                case DEPTH: data = this->renderer->getGBufferDepth(); break;
                case NORMAL: data = this->renderer->getGBufferNormal(); break;
                case COLOR: data = this->renderer->getGBufferColor(); break;
                default: assert(0); break;
            }
            this->imageDataMutex.lock();
            this->imageData = data;
            this->imageDataProgress = 1.0f;
            this->imageDataSignal = true;
            this->imageDataMutex.unlock();
        }
        else {
            for (int i = 0; i < this->config->sampleCount; ++i) {
                this->renderer->render();
                uint8_t *data = nullptr;
                if (this->config->filterType == FILTER_NONE) data = this->renderer->getFramebuffer();
                else data = this->renderer->getFramebufferAfterFilter();

                this->imageDataMutex.lock();
                this->imageData = data;
                this->imageDataProgress = (float)(i + 1) / (float)this->config->sampleCount;
                this->imageDataSignal = true;
                this->imageDataMutex.unlock();
            }
        }

        this->start->setEnabled(true);
    });
    threadRun.detach();
}

void Application::showInit() {
    using namespace nanogui;

    if(image->childCount()) image->removeChild(0);

    mImageTexture = new GLTexture("../cache/init");
    mImageTexture->loadFile("../cache/init.png");
    imageView = new ImageView(image, mImageTexture->texture());

    performLayout();
}

void Application::showFramebuffer(uint8_t *data, int width, int height) {
    using namespace nanogui;

    if(image->childCount()) image->removeChild(0);

    mImageTexture = new GLTexture("framebuffer");
    mImageTexture->loadData(data, width, height);
    imageView = new ImageView(image, mImageTexture->texture());

    performLayout();
}

void Application::drawAll() {
    if (imageDataSignal) {
        imageDataMutex.lock();
        showFramebuffer(imageData, SCREEN_WIDTH, SCREEN_HEIGHT);
        progressBar->setValue(imageDataProgress);
        imageDataSignal = false;
        imageDataMutex.unlock();
    }

    Screen::drawAll();
}