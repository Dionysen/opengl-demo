#pragma once

#include "Camera.h"
#include "SceneObject.h"
#include "Sun.h"
#include "FBO.h"
#include "Texture2D.h"

class Terrain : public SceneObject
{
public:
  Terrain(Sun *sun);
  virtual ~Terrain();

  virtual void initialize();
  virtual void render(int passID);
  virtual void update(double deltaTime);

  virtual void keyCallback(const int key, const int scanCode,
                           const int action, const int mods);

  bool enableWireFrame;

  // material
  Shader *shader;

  Material *material1;
  Material *material2;
  Material *material3;

  void loadMaterial();

private:
  float displacementScale;
  float tessellatedTriWidth;
  glm::ivec2 gridsize;
  glm::vec2 size;
  int patchCount;
  unsigned int VAO;

  Camera *camera;
  Sun *sun;

  // depth pass stuff
  glm::mat4 sunProjectionMatrix;
  glm::mat4 bias;
};