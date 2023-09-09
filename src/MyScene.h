#pragma once
#include "Application.h"

class MyScene : public Application
{
public:
  virtual void initialize();
  virtual void render(); // 渲染内容

  int waveID;
  float cameraSpeed;
};