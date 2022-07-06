#ifndef VPATHTRACER_APPLICATION_H
#define VPATHTRACER_APPLICATION_H

#include <nanogui/nanogui.h>
#include "common.h"
#include "renderer.h"

using handleType = std::unique_ptr<uint8_t[], void(*)(void*)>;

class GLTexture {
public:
    GLTexture() = default;
    GLTexture(const std::string& textureName): mTextureName(textureName), mTextureId(0) {}
    ~GLTexture() noexcept {
        if (mTextureId)
            glDeleteTextures(1, &mTextureId);
    }

    GLuint texture() const { return mTextureId; }

    /**
    *  Load a file in memory and create an OpenGL texture.
    *  Returns a handle type (an std::unique_ptr) to the loaded pixels.
    */
    handleType loadFile(const std::string& fileName);
    handleType loadData(uint8_t *data, int w, int h);

private:
    std::string mTextureName;
    GLuint mTextureId;
};

class Application : public nanogui::Screen {
public:
    Application();
    void run();
    void showInit();
    void showFramebuffer(uint8_t *data, int width, int height);

private:
    Renderer *renderer;
    Config *config;

    nanogui::Button *start;
    nanogui::Window *image;
    nanogui::ImageView *imageView;
    GLTexture *mImageTexture;
};

#endif //VPATHTRACER_APPLICATION_H
