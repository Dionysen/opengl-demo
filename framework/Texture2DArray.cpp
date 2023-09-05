#include "Texture2DArray.h"

Texture2DArray::Texture2DArray() :
	Texture(GL_TEXTURE_2D_ARRAY) {
}

Texture2DArray::~Texture2DArray() {
}

void Texture2DArray::pushToGPU(bool deleteAfterPush) {
	glTextureStorage3D(textureID, mipmapLevels, images[0].format.sizedFormat, images[0].width, images[0].height, images.size()); //allocate space for all

	for (unsigned int i = 0; i < images.size(); ++i) {
		ImageData &imageData = images[i];

		if (imageData.data != 0)
			glTextureSubImage3D(textureID,
				0,
				0, 0, i,
				imageData.width, imageData.height, 1,
				imageData.format.baseFormat, imageData.format.type, imageData.data);

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

	glTextureParameterf(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameterf(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}