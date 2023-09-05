#include "TextureCube.h"
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

TextureCube::TextureCube(bool invertYFaces) :
Texture(GL_TEXTURE_CUBE_MAP), invertYFaces(invertYFaces) {
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

TextureCube::~TextureCube() {
}

void TextureCube::initFromFile(const std::vector<std::string> &faces, const ImageFormat &format) {
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (std::string face : faces)
	{
		images.push_back(loadFile(face, format));
	}

	pushToGPU();
}

void TextureCube::pushToGPU(bool deleteAfterPush) {

	glTextureStorage2D(textureID, mipmapLevels, images[0].format.sizedFormat, images[0].width, images[0].height); //allocate space for all
	bind();

	for (int i = 0; i < 6; ++i) {
		ImageData &imageData = images[i];

		if (imageData.data != 0)
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, imageData.width, imageData.height, imageData.format.baseFormat, imageData.format.type, imageData.data); //level 0 mipmap

		if (deleteAfterPush) {
			delete[] imageData.data;
			imageData.data = 0;
		}
	}

	if (mipmapLevels > 1) {
		glGenerateTextureMipmap(textureID); //generate the remaining mipmaps
		glTextureParameterf(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
		glTextureParameterf(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTextureParameterf(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameterf(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameterf(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameterf(textureID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}