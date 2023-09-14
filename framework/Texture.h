#pragma once
#include "Shader.h"
#include <GL/glew.h>
#include <vector>

#include <IL/il.h>

struct ImageFormat
{
    unsigned int sizedFormat, baseFormat, type;
    int          numberOfChannels, bytesPerChannel;

    const static ImageFormat IMAGE_FORMAT_SRGBA8;
    const static ImageFormat IMAGE_FORMAT_SRGB8;

    const static ImageFormat IMAGE_FORMAT_SBGR8;

    const static ImageFormat IMAGE_FORMAT_RGBA8_LINEAR;
    const static ImageFormat IMAGE_FORMAT_RGB8_LINEAR;

    const static ImageFormat IMAGE_FORMAT_R8;
    const static ImageFormat IMAGE_FORMAT_R16;

    const static ImageFormat IMAGE_FORMAT_DEPTH_32F;

    ImageFormat(GLenum sizedFormat, GLenum baseFormat, GLenum type, uint32_t numberOfChannels, uint32_t bytesPerChannel)
        : sizedFormat(sizedFormat), baseFormat(baseFormat), type(type), numberOfChannels(numberOfChannels), bytesPerChannel(bytesPerChannel)
    {
    }

    ImageFormat() : sizedFormat(GL_SRGB8), baseFormat(GL_RGB), type(GL_UNSIGNED_BYTE), numberOfChannels(3), bytesPerChannel(1) {}
};

struct ImageData
{
    ImageFormat    format;
    int            width, height;
    unsigned char* data;
    unsigned int   sizeInBytes;
};

class Texture
{
  public:
    Texture(unsigned int type);

    virtual ~Texture();

    virtual void create();
    virtual void destroy();

    virtual void bind() const;
    virtual void pushToGPU(bool deleteAfterPush = false) {}

    void bind(int unit) const;
    void changeTextureParameter(GLenum parameter, GLfloat value);

    virtual const void* getTexel(const ImageData& imageData, const int x, const int y, const int z);
    virtual void        setTexel(int imageIndex, const int x, const int y, const int z, const void* value);

    virtual void initFromFile(const std::string& path, const ImageFormat& format = ImageFormat::IMAGE_FORMAT_SRGB8);
    virtual void initFromFiles(const std::vector<std::string>& paths, const ImageFormat& format = ImageFormat::IMAGE_FORMAT_SRGB8);

    void initFromData(uint32_t width, uint32_t height, const uint8_t* data, const ImageFormat& format = ImageFormat::IMAGE_FORMAT_SRGB8);

    inline void setMipmapLevels(int levels) { mipmapLevels = levels; }

    inline const ImageData&   getImageData(int index) { return images[index]; }
    inline const unsigned int getTextureID() { return textureID; }

    // unused func
    const unsigned int loadTextures(const std::vector<std::string>& files);
    const unsigned int loadTexture2D(const std::string file);

    unsigned int loadCubemap(std::vector<std::string> faces);
    unsigned int generateTexture2D(int w, int h);
    unsigned int generateTexture3D(int w, int h, int d);
    unsigned int type, textureID;

  protected:
    const ImageData loadFile(const std::string& path, const ImageFormat& format);

    int mipmapLevels;

    std::vector<ImageData> images;
};
