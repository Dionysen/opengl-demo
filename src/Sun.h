#pragma once

#include "Camera.h"
#include "DirectionalLight.h"
#include "Model.h"
#include "SceneObject.h"

class Sun : public SceneObject
{
public:
  Sun(float x, float y, float z);
  ~Sun();

  virtual void initialize();
  virtual void render(int passID);
  virtual void update(double deltaTime);

  virtual void keyCallback(const int key, const int scanCode,
                           const int action, const int mods);

  inline DirectionalLight &getDirectionalLight() { return directionalLight; }

  float DISTANCE = 800.0f;

private:
  DirectionalLight directionalLight;
  void computeDirection();
  Model *ourModel;
  Camera *camera;
};
