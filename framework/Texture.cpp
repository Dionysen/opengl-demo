#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

#include <iostream>

const ImageFormat ImageFormat::IMAGE_FORMAT_SRGBA8 = ImageFormat(GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE, 4, 1);
const ImageFormat ImageFormat::IMAGE_FORMAT_SRGB8 = ImageFormat(GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE, 3, 1);

const ImageFormat ImageFormat::IMAGE_FORMAT_SBGR8 = ImageFormat(GL_SRGB8, GL_BGR, GL_UNSIGNED_BYTE, 3, 1);

const ImageFormat ImageFormat::IMAGE_FORMAT_RGBA8_LINEAR = ImageFormat(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 4, 1);
const ImageFormat ImageFormat::IMAGE_FORMAT_RGB8_LINEAR = ImageFormat(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, 3, 1);

const ImageFormat ImageFormat::IMAGE_FORMAT_R8 = ImageFormat(GL_R8, GL_RED, GL_UNSIGNED_BYTE, 1, 1);
const ImageFormat ImageFormat::IMAGE_FORMAT_R16 = ImageFormat(GL_R16, GL_RED, GL_UNSIGNED_SHORT, 1, 2);

const ImageFormat ImageFormat::IMAGE_FORMAT_DEPTH_32F =
    ImageFormat(GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, 1, 4);

Texture::Texture(unsigned int type) : type(type), mipmapLevels(8)
{
    create();
}
Texture::~Texture()
{
}

void Texture::create()
{
    glCreateTextures(type, 1, &textureID);
}

void Texture::destroy()
{
    glDeleteTextures(1, &textureID);
}

void Texture::changeTextureParameter(GLenum parameter, GLfloat value)
{
    glTexParameteri(GL_TEXTURE_2D, parameter, value);
}

void Texture::bind(int unit) const
{
    glBindTextureUnit(unit, textureID);
}

void Texture::bind() const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(type, textureID);
}

void Texture::initFromFile(const std::string& path, const ImageFormat& format)
{

    ImageData imagedata = loadFile(path, format);

    if (imagedata.data)
    {
        images.push_back(imagedata);
        pushToGPU();
    }
    else
        std::cout << "init failed ...\n";
}

void Texture::initFromFiles(const std::vector<std::string>& paths, const ImageFormat& format)
{
    for (std::string path : paths)
    {
        images.push_back(loadFile(path, format));
    }
    pushToGPU();
}

void Texture::initFromData(uint32_t width, uint32_t height, const uint8_t* data, const ImageFormat& format)
{

    ImageData imageData;
    imageData.format = format;

    imageData.width = width;
    imageData.height = height;
    imageData.sizeInBytes = width * height * format.numberOfChannels * format.bytesPerChannel;

    if (data != 0)
    {
        imageData.data = new uint8_t[imageData.sizeInBytes];
        memcpy(imageData.data, data, imageData.sizeInBytes);
    }
    else
        imageData.data = 0;

    images.push_back(imageData);

    pushToGPU();
}

const ImageData Texture::loadFile(const std::string& path, const ImageFormat& format)
{

    // stb_image

    // ImageData imageData;
    // imageData.format = format;
    // int nrComponents;

    // unsigned char* data = stbi_load(path.c_str(), &imageData.width, &imageData.height, &nrComponents, 0);
    //// check format
    // if (nrComponents != imageData.format.numberOfChannels)
    //{
    //     std::cout << "Failed to load image from file: " << path << "[Format error!\n]" << std::endl;
    // }
    //// check data
    // if (data)
    //{
    //     imageData.sizeInBytes = imageData.width * imageData.height * imageData.format.numberOfChannels;
    //     imageData.data = new unsigned char[imageData.sizeInBytes];
    //     memcpy(imageData.data, data, imageData.sizeInBytes);
    // }
    // else
    //{
    //     std::cout << "Failed to load image from file: " << path << std::endl;
    // }
    // return imageData;

    // DevIL

    ilInit();

    ILuint image;
    ilGenImages(1, &image);
    ilBindImage(image);
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

    ImageData imageData;

    ILboolean result = ilLoadImage(path.c_str());
    if (result)
    {
        // ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

        imageData.format = format;

        imageData.width = ilGetInteger(IL_IMAGE_WIDTH);
        imageData.height = ilGetInteger(IL_IMAGE_HEIGHT);
        imageData.sizeInBytes =
            imageData.width * imageData.height * imageData.format.numberOfChannels * imageData.format.bytesPerChannel;

        imageData.data = new uint8_t[imageData.sizeInBytes];
        memcpy(imageData.data, ilGetData(), imageData.sizeInBytes);
    }
    else
    {
        std::cout << "Failed to load image from file: " << path << std::endl;
    }

    ilDeleteImages(1, &image);

    return imageData;
}

const void* Texture::getTexel(const ImageData& imageData, const int x, const int y, const int z)
{
    uint32_t texelSize = imageData.format.bytesPerChannel * imageData.format.numberOfChannels;
    uint8_t* ptr = imageData.data + (y * texelSize * imageData.width) + (x * texelSize);
    return ptr;
}

void Texture::setTexel(int imageIndex, const int x, const int y, const int z, const void* value)
{
    const ImageData& imageData = images[imageIndex];
    uint32_t texelSize = imageData.format.bytesPerChannel * imageData.format.numberOfChannels;
    uint8_t* ptr = imageData.data + (y * texelSize * imageData.width) + (x * texelSize);

    memcpy(ptr, value, texelSize);
}

unsigned int Texture::loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                         data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

unsigned int Texture::generateTexture2D(int w, int h)
{
    unsigned int tex_output;
    glGenTextures(1, &tex_output);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_output);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    return tex_output;
}

unsigned int Texture::generateTexture3D(int w, int h, int d)
{
    unsigned int tex_output;
    glGenTextures(1, &tex_output);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, tex_output);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexStorage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, w, h, d);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, w, h, d, 0, GL_RGBA, GL_FLOAT, NULL);
    glGenerateMipmap(GL_TEXTURE_3D);
    glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

    return tex_output;
}

const unsigned int Texture::loadTextures(const std::vector<std::string>& files)
{
    return 0;
}

const unsigned int Texture::loadTexture2D(const std::string path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        unsigned int format{};
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); //
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
