#pragma once
#include "Camera.h"
#include "SceneObject.h"
#include "Wave.h"

class WaveObject : public SceneObject
{

public:
  WaveObject();
  ~WaveObject(); // 删除内存

  virtual void initialize();
  virtual void render(int passID); // 渲染内容
  void buildTessendorfWaveMesh(Wave *wave_model);

  Camera *camera;

  glm::vec3 lightPos;

  unsigned int surfaceVAO, EBO, surfaceVBO;

  // Mesh resolution
  int N = 64;
  int M = 64;
  float L_x = 1000;
  float L_z = 1000;

  float times = 0;
  Wave *wave_model;

  int nWaves = 60;
  glm::vec4 *waveParams;

  float height;
  float modelScale;
  float heightMax = 0;
  float heightMin = 0;

  // Wind strengh
  float A = 3e-7f;
  // Wind speed
  float V = 30;
  // Wind direction
  glm::vec2 omega;

  int indexSize;
  glm::vec3 seacolor;
};