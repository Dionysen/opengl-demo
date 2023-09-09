#pragma once
#include "Camera.h"
#include "ModelAnimation.h"
#include "SceneObject.h"
class NanoSuit : public SceneObject
{
public:
  NanoSuit();
  ~NanoSuit(); // 删除内存

  virtual void initialize();
  virtual void render(int passID); // 渲染内容

  Camera *camera;
  AnimationModel *ourModel;
  glm::vec3 lightPos;
};