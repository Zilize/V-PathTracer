#include <nanogui/nanogui.h>
#include <iostream>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define SCREEN_WIDTH 784
#define SCREEN_HEIGHT 784
#define FORM_WIDTH 220
#define VALUE_WIDTH 80
#define MARGIN 10
#define PAD_WIDTH 32
#define PAD_HEIGHT 54

using std::cout;
using std::endl;

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

int sample_num = 16;
material_enum material = DIFFUSE;
sample_way_enum sample_way = UNIFORM;
float roughness = 0.5;
accel_structure_enum accel_structure = ACCEL_NONE;
filter_type_enum filter_type = FILTER_NONE;


class GLTexture {
public:
    using handleType = std::unique_ptr<uint8_t[], void(*)(void*)>;
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
    handleType load(const std::string& fileName) {
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

private:
    std::string mTextureName;
    GLuint mTextureId;
};


class Application : public nanogui::Screen {
public:
    Application() : nanogui::Screen(nanogui::Vector2i(SCREEN_WIDTH + FORM_WIDTH + PAD_WIDTH + MARGIN, SCREEN_HEIGHT + PAD_HEIGHT),
                                    "V-PathTracer", false) {
        using namespace nanogui;

        FormHelper *form = new FormHelper(this);
        ref<Window> setting = form->addWindow(Eigen::Vector2i(0, 0), "Settings");
        setting->setFixedWidth(FORM_WIDTH);

        form->addGroup("Rendering Settings");
        auto vSampleNum = form->addVariable("Sample Number", sample_num);
        vSampleNum->setFixedWidth(VALUE_WIDTH);
        vSampleNum->setSpinnable(true);
        vSampleNum->setFormat("[1-9][0-9]*");
        vSampleNum->setMinMaxValues(1, 256);

        auto vMaterial = form->addVariable("Material", material, true);
        vMaterial->setFixedWidth(VALUE_WIDTH);
        vMaterial->setItems({
            "Diffuse",
            "Microfacet"
        });

        auto vSampleWay = form->addVariable("Sample Way", sample_way, true);
        vSampleWay->setFixedWidth(VALUE_WIDTH);
        vSampleWay->setItems({
            "Uniform",
            "Cosine"
        });

        auto vRoughness = form->addVariable("Roughness", roughness, false);
        vRoughness->setFixedWidth(VALUE_WIDTH);
        vRoughness->setSpinnable(true);
        vRoughness->setMinMaxValues(0.0, 1.0);
        vRoughness->setValueIncrement(0.1);

        form->addGroup("Optimization Settings");
        auto vAccelStructure = form->addVariable("Accel Structure", accel_structure, true);
        vAccelStructure->setFixedWidth(VALUE_WIDTH);
        vAccelStructure->setItems({
            "None",
            "BVH",
            "SAH"
        });

        auto vFilterType = form->addVariable("Filter Type", filter_type, true);
        vFilterType->setFixedWidth(VALUE_WIDTH);
        vFilterType->setItems({
            "None",
            "Gauss",
            "Bilateral",
            "Joint"
        });

        form->addButton("Start", []() {
            cout << "Sample Number: " << sample_num << endl
                 << "Material: " << material << endl
                 << "Sample Way: " << sample_way << endl
                 << "Roughness: " << roughness << endl
                 << "Accel Structure: " << accel_structure << endl
                 << "Filter Type: " << filter_type << endl;
        });

        // Rendering Results

        auto image = new Window(this, "Rendering Result");
        image->setPosition(Vector2i(FORM_WIDTH + MARGIN, 0));
        image->setLayout(new GroupLayout());
        image->setFixedWidth(SCREEN_WIDTH + PAD_WIDTH);
        image->setFixedHeight(SCREEN_HEIGHT + PAD_HEIGHT);

        std::vector<std::pair<int, std::string>> framebuffers = loadImageDirectory(mNVGContext, "../cache");
        for (auto& framebuffer : framebuffers) {
            GLTexture texture(framebuffer.second);
            auto data = texture.load(framebuffer.second + ".png");
            mImagesData.emplace_back(std::move(texture), std::move(data));
        }
        auto imageView = new ImageView(image, mImagesData[0].first.texture());

        performLayout();

    }

private:
    using imagesDataType = std::vector<std::pair<GLTexture, GLTexture::handleType>>;
    imagesDataType mImagesData;
};

nanogui::ref<Application> app;

int main(int /* argc */, char ** /* argv */) {
    try {
        nanogui::init();

        /* scoped variables */ {
            app = new Application();
            app->drawAll();
            app->setVisible(true);
            nanogui::mainloop();
        }

        nanogui::shutdown();
    } catch (const std::runtime_error &e) {
        std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
        std::cerr << error_msg << std::endl;
        return -1;
    }

    return 0;
}