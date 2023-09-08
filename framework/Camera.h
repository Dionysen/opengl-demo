#ifndef CAMERA_H
#define CAMERA_H

#pragma once

#include "Application.h"
#include "Frustum.h"
#include "SceneObject.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class Application;
class SceneObject;

// 定义一些可能用到的常量
enum Camera_Movement
{
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  UP,
  DOWN,
  FASTER_FORWARD,
  FASTER_BACKWARD,
  FASTER_LEFT,
  FASTER_RIGHT,
  FASTER_UP,
  FASTER_DOWN
};

// 摄像机默认值
const float YAW = -90.0f;       // 偏航角度
const float PITCH = 0.0f;       // 上仰角度
const float SPEED = 5.0f;       // 摄像机移动速度，虽然实际是整个空间的物体同时在移动
const float SENSITIVITY = 0.1f; // 鼠标灵敏度，用以计算镜头转向
const float ZOOM = 45.0f;       // 视野，观察空间的大小

// An abstract camera class that processes input and calculates the
// corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera : public SceneObject
{
public:
  // 使用一个向量创建摄像机：
  // 主要参数为：位置，默认为原点;上向量，默认为010；方向向量为00-1,
  // 其他均可以为默认
  // Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW,
         float pitch = PITCH);
  // 使用标量创建摄像机
  // 主要参数为：位置，默认为原点;上向量，默认为010；方向向量为00-1,
  // 其他均可以为默认
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
         float yaw, float pitch);
  // returns the view matrix calculated using Euler Angles and the LookAt
  // Matrix
  glm::mat4 getViewMatrix(); // 生成观察矩阵
  glm::mat4 getProjectionMatrix();

  inline Frustum &getFrustum() { return frustum; }

  virtual void windowResizedCallback(const int width, const int height);
  virtual void keyCallback(const int key, const int scanCode,
                           const int action, const int mods);
  virtual void cursorPositionCallback(const double xpos, const double ypos);
  virtual void mouseScrollCallback(double xoffset, double yoffset);
  void ProcessKeyboard(Camera_Movement direction, float deltaTime);

  virtual void update(double deltaTime);

  inline glm::vec3 getPosition() { return Position; }
  inline void setSpeed(float speed) { MovementSpeed = speed; }
  inline void setApplication(Application *app) { application = app; }
  float Zoom;
  bool isCursorEnabled;

  void setProjectionMatrix(float fovy, float aspectRatio, float near, float far);
  void setPositionAndLookAt(const glm::vec3& position, const glm::vec3& lookAt, const glm::vec3& upVector);

  void setPosition(float x, float y, float z);
  void setLookAt(float x, float y, float z);

private:
  glm::vec3 lookAt;
  glm::vec3 up;
  void updateInternals();
  // 摄像机属性
  glm::vec3 Position; // 摄像机位置向量
  glm::vec3 Front;    // 方向向量，摄像机指向的目标的方向
  glm::vec3 Up;       // 上向量，也即y轴正方向，叉乘方向向量可得右向量
  glm::vec3 Right;    // 右向量，摄像机空间x轴的正方向
  glm::vec3 WorldUp;  // 上向量
  // 有了三个互相垂直的轴，外加一个平移向量，即可创建一个矩阵，可以用这个矩阵乘以任何向量来将其变换到那个空间

  // euler Angles
  float Yaw;
  float Pitch;
  // camera options
  float MovementSpeed;
  float MouseSensitivity;

  float lastX = 1280 / 2.0f;
  float lastY = 800 / 2.0f;
  bool firstMouse = true;

  Application *application;
  Frustum frustum;
  glm::mat4 projectionMatrix;
  bool projectionMatrixDirtyFlag;
  // 从更新后的相机的欧拉角计算方向向量
  void updateCameraVectors();
};
#endif
