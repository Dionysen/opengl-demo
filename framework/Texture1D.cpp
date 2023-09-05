#include "Texture1D.h"

Texture1D::Texture1D() :
	Texture(GL_TEXTURE_1D) {
}

Texture1D::~Texture1D() {
}

void Texture1D::pushToGPU(bool deleteAfterPush) {
	ImageData &imageData = images[0];

	glTextureStorage1D(textureID, mipmapLevels, imageData.format.sizedFormat, imageData.width); //allocate space for all

	if (imageData.data != 0)
		glTextureSubImage1D(textureID, 0, 0, imageData.width, imageData.format.baseFormat, imageData.format.type, imageData.data); //level 0 mipmap

	if (mipmapLevels > 1) {
		glGenerateTextureMipmap(textureID); //generate the remaining mipmaps
		glTextureParameterf(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
		glTextureParameterf(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTextureParameterf(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameterf(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameterf(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (deleteAfterPush) {
		delete[] imageData.data;
		imageData.data = 0;
	}
}