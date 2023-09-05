#pragma once
#include "Shader.h"
#include "Texture.h"

#include <vector>

class Material {
  public:
    Material();
    virtual ~Material();

    void setShader(unsigned int index, Shader *shader);
    void setTexture(unsigned int index, Texture *texture);

    inline Shader *getShader(int id) const { return shaders[id]; }
    inline Texture *getTexture(int id) const { return textures[id]; }

    inline int getTextureCount() { return static_cast<int>(textures.size()); }
    inline void setActiveShader(int s) { activeShader = s; }
    inline Shader *getActiveShader() { return shaders[activeShader]; }

    void bind() const;
    void bindTextures() const;
    void useShader() const;

  private:
    std::vector<Shader *> shaders;
    std::vector<Texture *> textures;

    int activeShader;
};