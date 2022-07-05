#ifndef VPATHTRACER_APPLICATION_H
#define VPATHTRACER_APPLICATION_H

#include <nanogui/nanogui.h>
#include <common.h>

using handleType = std::unique_ptr<uint8_t[], void(*)(void*)>;

class GLTexture {
public:
    GLTexture() = default;
    GLTexture(const std::string& textureName)
            : mTextureName(textureName), mTextureId(0) {}

    GLTexture(const std::string& textureName, GLint textureId)
            : mTextureName(textureName), mTextureId(textureId) {}

    GLTexture(const GLTexture& other) = delete;
    GLTexture(GLTexture&& other) noexcept
            : mTextureName(std::move(other.mTextureName)),
              mTextureId(other.mTextureId) {
        other.mTextureId = 0;
    }
    GLTexture& operator=(const GLTexture& other) = delete;
    GLTexture& operator=(GLTexture&& other) noexcept {
        mTextureName = std::move(other.mTextureName);
        std::swap(mTextureId, other.mTextureId);
        return *this;
    }
    ~GLTexture() noexcept {
        if (mTextureId)
            glDeleteTextures(1, &mTextureId);
    }

    GLuint texture() const { return mTextureId; }
    const std::string& textureName() const { return mTextureName; }

    /**
    *  Load a file in memory and create an OpenGL texture.
    *  Returns a handle type (an std::unique_ptr) to the loaded pixels.
    */
    handleType load(const std::string& fileName);

private:
    std::string mTextureName;
    GLuint mTextureId;
};

class Application : public nanogui::Screen {
public:
    Application();
    void showPicture();

private:
    nanogui::Window *image;
    nanogui::ImageView *imageView;
    std::vector<std::pair<int, std::string>> framebuffers;
    GLTexture *mImageTexture;
};

#endif //VPATHTRACER_APPLICATION_H
