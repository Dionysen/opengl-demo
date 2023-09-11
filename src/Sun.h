#pragma once

#include "Camera.h"
#include "DirectionalLight.h"
#include "Model.h"
#include "SceneObject.h"
#include "Sun.h"
#include <glm/gtc/type_ptr.hpp>

#include "Application.h"

class Sun : public SceneObject
{
public:
  Sun(float x, float y, float z) : directionalLight(x, y, z) {}
  ~Sun() {}

  virtual void initialize()
  {
    std::cout << "Init the sun ...\n";
    transform.setPosition(DISTANCE, 0.0f, 0.0f, WORLD);
    transform.setRotation(0.0f, 90.0f, 0.0f, LOCAL, LOCAL);
    transform.rotateBy(0.0f, 0.0f, 50.0f, WORLD, WORLD);

    computeDirection();

    Shader *shader = new Shader("shaders/white.vert", "shaders/white.frag");
    Material *material = new Material();
    material->setShader(0, shader);
    material->setActiveShader(0);
    material->bind();
    setMaterial(material);

    camera = &application->getActiveCamera();
    ourModel = new Model("meshes/sphere.obj");

    application->addEventListener(this);
    std::cout << "The sun initialized.\n";
  }

  virtual void render(int passID)
  {
    material->useShader();

    static float zoom = 10.0f;
    static float vel = 50.0f;
    transform.setScale(zoom, zoom, zoom, WORLD);

    if (ImGui::CollapsingHeader("Sun"))
    {
      ImGui::DragFloat("Sun size", &zoom, 0.01f, 5.0f, 20.0f, "%.3f", 0);
      transform.setScale(zoom, zoom, zoom, WORLD);

      if (ImGui::SliderFloat("Distance", &DISTANCE, 400.0f, 1000.0f))
      {
        transform.setPosition(DISTANCE, 0.0f, 0.0f, WORLD);
        transform.setRotation(0.0f, 90.0f, 0.0f, LOCAL, LOCAL);
        transform.rotateBy(0.0f, 0.0f, 50.0f, WORLD, WORLD);
        computeDirection();
      }
    }

    material->getActiveShader()->setMat4(
        "mvp",
        glm::mat4(camera->getProjectionMatrix() * camera->getViewMatrix() * transform.getTransformationMatrix()));
    ourModel->Draw(*material->getActiveShader());
  }

  virtual void update(double deltaTime) {}

  virtual void keyCallback(const int key, const int scanCode,
                           const int action, const int mods)
  {
    const float VEL = 1.0f;

    if (key == GLFW_KEY_LEFT)
    {
      transform.rotateBy(0.0f, 0.0f, VEL, WORLD, WORLD);
      computeDirection();
    }
    else if (key == GLFW_KEY_RIGHT)
    {
      transform.rotateBy(0.0f, 0.0f, -VEL, WORLD, WORLD);
      computeDirection();
    }
  }

  inline DirectionalLight &getDirectionalLight() { return directionalLight; }
  float DISTANCE = 800.0f;

private:
  DirectionalLight directionalLight;
  void computeDirection()
  {
    const glm::vec3 &dir = -transform.getPosition();
    directionalLight.setDirection(dir.x, dir.y, dir.z);
  }

  Model *ourModel;
  Camera *camera;
};
