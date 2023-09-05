#include "Material.h"

Material::Material() : activeShader(0) {}

Material::~Material() {}

void Material::setShader(unsigned int index, Shader *shader) {
    if (index >= this->shaders.size())
        this->shaders.resize(index + 1);
    this->shaders[index] = shader;
}

void Material::setTexture(unsigned int index, Texture *texture) {
    if (index >= textures.size())
        textures.resize(index + 1);
    textures[index] = texture;
}

void Material::bind() const {
    useShader();
    bindTextures();
}

void Material::bindTextures() const {
    for (unsigned int i = 0; i < textures.size(); ++i) {
        textures[i]->bind(i);
    }
}

void Material::useShader() const { this->shaders[activeShader]->use(); }