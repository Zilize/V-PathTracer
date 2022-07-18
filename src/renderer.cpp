#include <thread>

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "renderer.h"
#include "utils.h"

void Renderer::init(Config *_config) {
    clear();
    config = _config;
    scene = new Scene();

    // Init sampleFramebufferPool and framebuffer
    for (int i = 0; i < config->threadCount; ++i)
        sampleFramebufferPool.emplace_back(vector<vec3>());
    for (int i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH; ++i)
        framebuffer.emplace_back(vec3(0, 0, 0));

    // Load scene
    if (config->scene == BOX) scene->loadSceneBox(config);
    else scene->loadSceneBunny(config);

    // Set camera
    camera = new Camera(vec3(278, 273, -790), vec3(278, 273, 0), vec3(0, 1, 0), 38, 1);

    // Build accelerating structure
    switch (config->accelStructure) {
        case ACCEL_NONE: scene->buildNaive(); break;
        case BVH: scene->buildBVH(); break;
        case SAH: scene->buildSAH(); break;
        default: break;
    }

    if (config->gBuffer != GBUFFER_NONE || (config->filterType != FILTER_NONE && config->filterType != GAUSS)) {
        buildGBuffer();
    }
}

void Renderer::clear() {
    scene = nullptr;

    gBufferDepth.clear();
    gBufferNormal.clear();
    gBufferColor.clear();
    gBufferPosition.clear();
    sampleFramebufferPool.clear();
    framebuffer.clear();
    framebufferAfterFilter.clear();
    currentSampleCount = 0;
}

void Renderer::render() {
    for (auto & sampleFramebuffer : sampleFramebufferPool)
        sampleFramebuffer.clear();
    int blockLength = floor(1.0f * SCREEN_HEIGHT / (float)config->threadCount);

    threadPool.clear();
    for (int threadIndex = 0; threadIndex < config->threadCount; ++threadIndex) {
        std::thread t([this, threadIndex, blockLength]() {
            int begin = threadIndex * blockLength;
            int end = (threadIndex == config->threadCount) ? SCREEN_HEIGHT : (threadIndex + 1) * blockLength;
            for (int i = begin; i < end; ++i) {
                for (int j = 0; j < SCREEN_WIDTH; ++j) {
                    Ray ray = camera->getRayRandom(i, j);
                    vec3 pixel = scene->castRay(ray);
                    sampleFramebufferPool[threadIndex].emplace_back(pixel);
                }
            }
        });
        threadPool.emplace_back(std::move(t));
    }
    for (auto &t: threadPool) t.join();

    int pixelIndex = 0;
    for (auto &sampleFramebuffer: sampleFramebufferPool) {
        for (auto &pixel: sampleFramebuffer) {
            framebuffer[pixelIndex] = (float)currentSampleCount / ((float)currentSampleCount + 1.0f) * framebuffer[pixelIndex] +
                                      1.0f / ((float)currentSampleCount + 1.0f) * pixel;
            pixelIndex++;
        }
    }
    currentSampleCount++;
    cout << "SPP: " << currentSampleCount << endl;
}

void Renderer::buildGBuffer() {
    for (int i = 0; i < SCREEN_HEIGHT; ++i) {
        for (int j = 0; j < SCREEN_WIDTH; ++j) {
            float depth;
            vec3 normal, color, position;

            Ray ray = camera->getRayMiddle(i, j);
            if (scene->getGBufferInfo(ray, depth, normal, color, position)) {
                gBufferDepth.emplace_back(depth);
                gBufferNormal.emplace_back(normal);
                gBufferColor.emplace_back(color);
                gBufferPosition.emplace_back(position);
            }
            else {  // set background color for GBuffer
                gBufferDepth.emplace_back(-1.0);
                gBufferNormal.emplace_back(normalize(vec3(1, 1, 1)));
                gBufferColor.emplace_back(normalize(vec3(1, 1, 1)));
                gBufferPosition.emplace_back(vec3(0, 0, 0));
            }
        }
    }
}

void Renderer::filterByGauss() {
    int halfSize = (GAUSSIAN_FILTER_SIZE - 1) / 2;
    vector<float> weights;
    float weightSum = 0.0f;
    for (int i = 0; i < halfSize; ++i) {
        float value = gaussianFilter(i, GAUSSIAN_FILTER_SIGMA);
        weights.emplace_back(value);
        if (i == 0) weightSum += value;
        else weightSum += 2 * value;
    }
    for (auto &weight: weights) weight /= weightSum;

    // Horizontal Filtering
    vector<vec3> framebufferAfterHorizontal;
    for (int i = 0; i < SCREEN_HEIGHT; ++i) {
        for (int j = 0; j < SCREEN_WIDTH; ++j) {
            if (j - halfSize < 0 || j + halfSize >= SCREEN_WIDTH) {
                framebufferAfterHorizontal.emplace_back(framebuffer[i * SCREEN_WIDTH + j]);
                continue;
            }
            vec3 pixel(0, 0, 0);
            for (int col = j - halfSize; col <= j + halfSize; ++col) {
                int distance = (col - j) > 0 ? (col - j) : (j - col);
                pixel += weights[distance] * framebuffer[i * SCREEN_WIDTH + col];
            }
            framebufferAfterHorizontal.emplace_back(pixel);
        }
    }

    // Vertical Filtering
    for (int i = 0; i < SCREEN_HEIGHT; ++i) {
        for (int j = 0; j < SCREEN_WIDTH; ++j) {
            if (i - halfSize < 0 || i + halfSize >= SCREEN_HEIGHT) {
                framebufferAfterFilter.emplace_back(framebufferAfterHorizontal[i * SCREEN_WIDTH + j]);
                continue;
            }
            vec3 pixel(0, 0, 0);
            for (int row = i - halfSize; row <= i + halfSize; ++row) {
                int distance = (row - i) > 0 ? (row - i) : (i - row);
                pixel += weights[distance] * framebufferAfterHorizontal[row * SCREEN_WIDTH + j];
            }
            framebufferAfterFilter.emplace_back(pixel);
        }
    }
}

void Renderer::filterByBilateral() {
    int halfSize = (BILATERAL_FILTER_SIZE - 1) / 2;

#ifdef BILATERAL_FILTER_ACCELERATION
    // Horizontal Filtering
    vector<vec3> framebufferAfterHorizontal;
    for (int i = 0; i < SCREEN_HEIGHT; ++i) {
        for (int j = 0; j < SCREEN_WIDTH; ++j) {
            if (j - halfSize < 0 || j + halfSize >= SCREEN_WIDTH) {
                framebufferAfterHorizontal.emplace_back(framebuffer[i * SCREEN_WIDTH + j]);
                continue;
            }
            vec3 pixel(0, 0, 0);
            float weightSum = 0.0f;
            for (int col = j - halfSize; col <= j + halfSize; ++col) {
                vec3 colorDistance = framebuffer[i * SCREEN_WIDTH + col] - framebuffer[i * SCREEN_WIDTH + j];
                float weight = bilateralFilter(col - j, colorDistance, BILATERAL_FILTER_DISTANCE_SIGMA, BILATERAL_FILTER_COLOR_SIGMA);
                pixel += weight * framebuffer[i * SCREEN_WIDTH + col];
                weightSum += weight;
            }
            pixel /= weightSum;
            framebufferAfterHorizontal.emplace_back(pixel);
        }
    }

    // Vertical Filtering
    for (int i = 0; i < SCREEN_HEIGHT; ++i) {
        for (int j = 0; j < SCREEN_WIDTH; ++j) {
            if (i - halfSize < 0 || i + halfSize >= SCREEN_HEIGHT) {
                framebufferAfterFilter.emplace_back(framebufferAfterHorizontal[i * SCREEN_WIDTH + j]);
                continue;
            }
            vec3 pixel(0, 0, 0);
            float weightSum = 0.0f;
            for (int row = i - halfSize; row <= i + halfSize; ++row) {
                vec3 colorDistance = framebufferAfterHorizontal[row * SCREEN_WIDTH + j] - framebufferAfterHorizontal[i * SCREEN_WIDTH + j];
                float weight = bilateralFilter(row - i, colorDistance, BILATERAL_FILTER_DISTANCE_SIGMA, BILATERAL_FILTER_COLOR_SIGMA);
                pixel += weight * framebufferAfterHorizontal[row * SCREEN_WIDTH + j];
                weightSum += weight;
            }
            pixel /= weightSum;
            framebufferAfterFilter.emplace_back(pixel);
        }
    }
#else
    for (int i = 0; i < SCREEN_HEIGHT; ++i) {
        for (int j = 0; j < SCREEN_WIDTH; ++j) {
            if (i - halfSize < 0 || j - halfSize < 0 || i + halfSize >= SCREEN_HEIGHT || j + halfSize >= SCREEN_WIDTH) {
                framebufferAfterFilter.emplace_back(framebuffer[i * SCREEN_WIDTH + j]);
                continue;
            }
            vec3 pixel(0, 0, 0);
            float weightSum = 0.0f;
            for (int row = i - halfSize; row <= i + halfSize; ++row) {
                for (int col = j - halfSize; col <= j + halfSize; ++col) {
                    int distanceSquare = (row - i) * (row - i) + (col - j) * (col - j);
                    float colorDistance = length(framebuffer[row * SCREEN_WIDTH + col] - framebuffer[i * SCREEN_WIDTH + j]);
                    float variance = 2.0f * BILATERAL_FILTER_DISTANCE_SIGMA * BILATERAL_FILTER_DISTANCE_SIGMA;
                    float varianceColor = 2.0f * BILATERAL_FILTER_COLOR_SIGMA * BILATERAL_FILTER_COLOR_SIGMA;
                    float weight = powf(2.718281828f, - (float)distanceSquare / variance - colorDistance * colorDistance / varianceColor);
                    pixel += weight * framebuffer[row * SCREEN_WIDTH + col];
                    weightSum += weight;
                }
            }
            pixel /= weightSum;
            framebufferAfterFilter.emplace_back(pixel);
        }
    }
#endif
}

void Renderer::filterByJoint() {
    int halfSize = (JOINT_FILTER_SIZE - 1) / 2;
    float fixNorm = JOINT_FILTER_SIZE * JOINT_FILTER_SIZE;

    // Outline Removal
    vector<vec3> framebufferAfterRemoval;
    for (int i = 0; i < SCREEN_HEIGHT; ++i) {
        for (int j = 0; j < SCREEN_WIDTH; ++j) {
            if (i - halfSize < 0 || j - halfSize < 0 || i + halfSize >= SCREEN_HEIGHT || j + halfSize >= SCREEN_WIDTH) {
                framebufferAfterRemoval.emplace_back(framebuffer[i * SCREEN_WIDTH + j]);
                continue;
            }

            // calculate variance
            vec3 averageColor(0, 0, 0);
            for (int row = i - halfSize; row <= i + halfSize; ++row) {
                for (int col = j - halfSize; col <= j + halfSize; ++col) {
                    averageColor += framebuffer[row * SCREEN_WIDTH + col];
                }
            }
            averageColor /= JOINT_FILTER_SIZE * JOINT_FILTER_SIZE;

            vec3 varianceColor(0, 0, 0);
            for (int row = i - halfSize; row <= i + halfSize; ++row) {
                for (int col = j - halfSize; col <= j + halfSize; ++col) {
                    vec3 distanceColor = framebuffer[row * SCREEN_WIDTH + col] - averageColor;
                    varianceColor += distanceColor * distanceColor;
                }
            }
            varianceColor /= JOINT_FILTER_SIZE * JOINT_FILTER_SIZE;
            vec3 sigmaColor = glm::sqrt(varianceColor);

            vec3 pixel = glm::clamp(framebuffer[i * SCREEN_WIDTH + j], averageColor - 1.0f * sigmaColor, averageColor + 1.0f * sigmaColor);
            framebufferAfterRemoval.emplace_back(pixel);
        }
    }

    // Joint Bilateral Filtering
    for (int i = 0; i < SCREEN_HEIGHT; ++i) {
        for (int j = 0; j < SCREEN_WIDTH; ++j) {
            if (i - halfSize < 0 || j - halfSize < 0 || i + halfSize >= SCREEN_HEIGHT || j + halfSize >= SCREEN_WIDTH) {
                framebufferAfterFilter.emplace_back(framebufferAfterRemoval[i * SCREEN_WIDTH + j]);
                continue;
            }

            // calculate variance
            vec3 averageColor(0, 0, 0);
            for (int row = i - halfSize; row <= i + halfSize; ++row) {
                for (int col = j - halfSize; col <= j + halfSize; ++col) {
                    averageColor += framebufferAfterRemoval[row * SCREEN_WIDTH + col];
                }
            }
            averageColor /= JOINT_FILTER_SIZE * JOINT_FILTER_SIZE;

            vec3 varianceColor(0, 0, 0);
            for (int row = i - halfSize; row <= i + halfSize; ++row) {
                for (int col = j - halfSize; col <= j + halfSize; ++col) {
                    vec3 distanceColor = framebufferAfterRemoval[row * SCREEN_WIDTH + col] - averageColor;
                    varianceColor += distanceColor * distanceColor;
                }
            }
            varianceColor /= JOINT_FILTER_SIZE * JOINT_FILTER_SIZE;

            vec3 pixel(0, 0, 0);
            float weightSum = 0.0f;

            float weightSumDepth = 0.0f, weightSumNormal = 0.0f, weightSumColor = 0.0f;
            for (int row = i - halfSize; row <= i + halfSize; ++row) {
                for (int col = j - halfSize; col <= j + halfSize; ++col) {
                    // weight of depth
                    float distanceZ = gBufferDepth[i * SCREEN_WIDTH + j] - gBufferDepth[row * SCREEN_WIDTH + col];
                    distanceZ = distanceZ > 0.0f ? distanceZ : -distanceZ;
                    float gradientX = gBufferNormal[i * SCREEN_WIDTH + j].x / (gBufferNormal[i * SCREEN_WIDTH + j].z + 0.0005f);
                    float distanceX = gBufferPosition[i * SCREEN_WIDTH + j].x - gBufferPosition[row * SCREEN_WIDTH + col].x;
                    float predictX = gradientX * distanceX;
                    predictX = predictX > 0.0f ? predictX : -predictX;
                    float gradientY = gBufferNormal[i * SCREEN_WIDTH + j].y / (gBufferNormal[i * SCREEN_WIDTH + j].z + 0.0005f);
                    float distanceY = gBufferPosition[i * SCREEN_WIDTH + j].y - gBufferPosition[row * SCREEN_WIDTH + col].y;
                    float predictY = gradientY * distanceY;
                    predictY = predictY > 0.0f ? predictY : -predictY;
                    float weightDepth = powf(2.718281828f, - distanceZ / ((float)JOINT_FILTER_DEPTH_SIGMA * (predictX + predictY) + 0.0005f));

                    // weight of normal
                    float baseNormal = std::max(0.0f, dot(gBufferNormal[i * SCREEN_WIDTH + j], gBufferNormal[row * SCREEN_WIDTH + col]));
                    float weightNormal = powf(baseNormal, JOINT_FILTER_NORMAL_SIGMA);

                    // weight of color
                    float distanceColor = length(framebufferAfterRemoval[i * SCREEN_WIDTH + j] - framebufferAfterRemoval[row * SCREEN_WIDTH + col]);
                    float weightColor = powf(2.718281828f, - distanceColor / ((float)JOINT_FILTER_COLOR_SIGMA * sqrtf(length(varianceColor)) + 0.0005f));

                    float weight = weightDepth + weightNormal / fixNorm + weightColor / fixNorm;
                    pixel += weight * framebufferAfterRemoval[row * SCREEN_WIDTH + col];
                    weightSum += weight;

                    weightSumDepth += weightDepth;
                    weightSumNormal += weightNormal;
                    weightSumColor += weightColor;
                }
            }
            pixel /= weightSum;
            framebufferAfterFilter.emplace_back(pixel);
        }
    }
}

void Renderer::filterByOutlineRemoval() {
    int halfSize = (OUTLINE_REMOVAL_FILTER_SIZE - 1) / 2;
    float k = 1.0f;
    for (int i = 0; i < SCREEN_HEIGHT; ++i) {
        for (int j = 0; j < SCREEN_WIDTH; ++j) {
            if (i - halfSize < 0 || j - halfSize < 0 || i + halfSize >= SCREEN_HEIGHT || j + halfSize >= SCREEN_WIDTH) {
                framebufferAfterFilter.emplace_back(framebuffer[i * SCREEN_WIDTH + j]);
                continue;
            }

            // calculate variance
            vec3 averageColor(0, 0, 0);
            for (int row = i - halfSize; row <= i + halfSize; ++row) {
                for (int col = j - halfSize; col <= j + halfSize; ++col) {
                    averageColor += framebuffer[row * SCREEN_WIDTH + col];
                }
            }
            averageColor /= JOINT_FILTER_SIZE * JOINT_FILTER_SIZE;

            vec3 varianceColor(0, 0, 0);
            for (int row = i - halfSize; row <= i + halfSize; ++row) {
                for (int col = j - halfSize; col <= j + halfSize; ++col) {
                    vec3 distanceColor = framebuffer[row * SCREEN_WIDTH + col] - averageColor;
                    varianceColor += distanceColor * distanceColor;
                }
            }
            varianceColor /= JOINT_FILTER_SIZE * JOINT_FILTER_SIZE;
            vec3 sigmaColor = glm::sqrt(varianceColor);

            vec3 pixel = glm::clamp(framebuffer[i * SCREEN_WIDTH + j], averageColor - k * sigmaColor, averageColor + k * sigmaColor);
            framebufferAfterFilter.emplace_back(pixel);
        }
    }
}

uint8_t *Renderer::getGBufferDepth() {
    float minDepth = std::numeric_limits<float>::max();
    float maxDepth = 0.0f;
    for (auto depth: gBufferDepth) {
        if (depth > maxDepth) maxDepth = depth;
        if (depth < minDepth && depth > 0.0f) minDepth = depth;
    }
    float range = maxDepth - minDepth;
    vector<vec3> gBufferDepthVisual;
    for (auto depth: gBufferDepth) {
        float grayValue = (depth - minDepth) / range;
        gBufferDepthVisual.emplace_back(vec3(grayValue, grayValue, grayValue));
    }
    return dumpData(gBufferDepthVisual);
}

uint8_t *Renderer::getGBufferNormal() {
    vec3 oneVec(1.0, 1.0, 1.0);
    vector<vec3> gBufferNormalVisual;
    for (auto normal: gBufferNormal) {
        gBufferNormalVisual.emplace_back((float)0.5 * (normal + oneVec));
    }
    return dumpData(gBufferNormalVisual);
}

uint8_t *Renderer::getGBufferColor() {
    return dumpData(gBufferColor);
}

uint8_t *Renderer::getGBufferPosition() {
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float maxY = std::numeric_limits<float>::min();
    float maxZ = std::numeric_limits<float>::min();
    for (auto &position: gBufferPosition) {
        if (position.x < minX) minX = position.x;
        if (position.y < minY) minY = position.y;
        if (position.z < minZ) minZ = position.z;
        if (position.x > maxX) maxX = position.x;
        if (position.y > maxY) maxY = position.y;
        if (position.z > maxZ) maxZ = position.z;
    }
    float rangeX = maxX - minX;
    float rangeY = maxY - minY;
    float rangeZ = maxZ - minZ;
    vector<vec3> gBufferPositionVisual;
    for (auto &position: gBufferPosition) {
        float xValue = (position.x - minX) / rangeX;
        float yValue = (position.y - minY) / rangeY;
        float zValue = (position.z - minZ) / rangeZ;
        gBufferPositionVisual.emplace_back(vec3(xValue, yValue, zValue));
    }
    return dumpData(gBufferPositionVisual);
}

uint8_t *Renderer::getFramebuffer() {
    return dumpData(framebuffer);
}

uint8_t *Renderer::getFramebufferAfterFilter() {
    framebufferAfterFilter.clear();
    switch (config->filterType) {
        case GAUSS: filterByGauss(); break;
        case BILATERAL: filterByBilateral(); break;
        case JOINT: filterByJoint(); break;
        case OUTLINE: filterByOutlineRemoval(); break;
        default: assert(0); break;
    }
    return dumpData(framebufferAfterFilter);
}

void Renderer::dumpFramebuffer() {
    dumpFile(framebuffer, "../cache/framebuffer.png");
}

void Renderer::dumpFramebufferAfterFilter() {
    dumpFile(framebufferAfterFilter, "../cache/framebufferAfterFilter.png");
}

void Renderer::dumpFile(vector<vec3> &buffer, const string &filename) {
    assert(buffer.size() == SCREEN_WIDTH * SCREEN_HEIGHT);
    uint8_t *data = new uint8_t[SCREEN_WIDTH * SCREEN_HEIGHT * 3];

    int index = 0;
    for (auto pixel: buffer) {
        data[index++] = static_cast<uint8_t>(255 * std::powf(glm::clamp(pixel.x, 0.0f, 1.0f), 0.6f));
        data[index++] = static_cast<uint8_t>(255 * std::powf(glm::clamp(pixel.y, 0.0f, 1.0f), 0.6f));
        data[index++] = static_cast<uint8_t>(255 * std::powf(glm::clamp(pixel.z, 0.0f, 1.0f), 0.6f));
    }
    stbi_write_png(filename.c_str(), SCREEN_WIDTH, SCREEN_HEIGHT, 3, data, SCREEN_WIDTH * 3);
    delete data;
}

uint8_t *Renderer::dumpData(vector<vec3> &buffer) {
    assert(buffer.size() == SCREEN_WIDTH * SCREEN_HEIGHT);
    uint8_t *data = new uint8_t[SCREEN_WIDTH * SCREEN_HEIGHT * 3];

    int index = 0;
    for (auto pixel: buffer) {
        data[index++] = static_cast<uint8_t>(255 * std::powf(glm::clamp(pixel.x, 0.0f, 1.0f), 0.6f));
        data[index++] = static_cast<uint8_t>(255 * std::powf(glm::clamp(pixel.y, 0.0f, 1.0f), 0.6f));
        data[index++] = static_cast<uint8_t>(255 * std::powf(glm::clamp(pixel.z, 0.0f, 1.0f), 0.6f));
    }
    return data;  // Todo: data not delete yet.
}
