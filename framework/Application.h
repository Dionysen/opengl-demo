#pragma once
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <string>
#include <vector>

#include "Camera.h"
#include "EventListener.h"
#include "ImguiEmbeded.h"
#include "SceneObject.h"

class SceneObject;
class Camera;
class Application : public EventListener
{
public:
  Application();
  ~Application();

  virtual void initialize();
  virtual void render();
  virtual void update(double deltaTime);

  virtual void processInput(GLFWwindow *window);
  virtual void windowResizedCallback(const int width, const int height);
  virtual void keyCallback(const int key, const int scanCode,
                           const int action, const int mods);
  virtual void cursorPositionCallback(const double xpos, const double ypos);
  virtual void mouseButtonCallback(const int button, const int action,
                                   const int mods);

  bool initGLContext(int samples = 0, bool srgbFrameBuffer = true);
  void startLoop();

  void addCamera(Camera *camera);
  int addSceneObject(SceneObject *sceneObject);
  int addEventListener(EventListener *eventListener);
  void setWindowProperties(const std::string &title, const int width,
                           const int height);

  inline int getWindowWidth() const { return SCR_WIDTH; }
  inline int getWindowHeight() const { return SCR_HEIGHT; }
  inline Camera &getCamera(int id) const { return *cameras[id]; }
  inline void setActiveCamera(int id) { activeCamera = id; }
  inline Camera &getActiveCamera() const { return *cameras[activeCamera]; }

  inline float getDeltaTime() const { return this->deltaTime; }

protected:
  GLFWwindow *window;
  int SCR_WIDTH, SCR_HEIGHT;
  std::string windowTitle;

  ImguiEmbeded *imguiEmbeded;

  std::vector<SceneObject *> sceneObjects;
  std::vector<EventListener *> eventListeners;
  std::vector<Camera *> cameras;

  int activeCamera;

  float deltaTime;
  float lastFrame;

  bool isCursorEnabled;
  bool isFullScreen;
  bool initialized;
  // bool vsyncEnabled;

  // inline void setVsync() { glfwSwapInterval(vsyncEnabled ? 1 : 0); }
};