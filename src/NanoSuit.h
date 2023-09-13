#pragma once
#include "Application.h"
#include "Camera.h"
#include "Material.h"
#include "Model.h"
#include "NanoSuit.h"
#include "SceneObject.h"
#include "Shader.h"
#include <iostream>

class NanoSuit : public SceneObject
{
  public:
    NanoSuit()
    {
    }
    ~NanoSuit()
    {
    }

    virtual void initialize()
    {
        std::cout << "Init nanosuit ...\n";
        Material* material = new Material();
        Shader* shader = new Shader("shaders/model.vert", "shaders/model.frag");
        material->setShader(0, shader);
        material->setActiveShader(0);
        material->useShader();

        setMaterial(material);
        material->bind();

        camera = &application->getActiveCamera();

        ourModel = new Model("textures/nanosuit/nanosuit.obj");
        std::cout << "The nanosuit initialized.\n";
    }

    virtual void render(int passID)
    {
        getMaterial().useShader();
        glm::mat4 view = camera->getViewMatrix();
        getMaterial().getShader(0)->setMat4("projection", camera->getProjectionMatrix());
        getMaterial().getShader(0)->setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.0));
        getMaterial().getShader(0)->setMat4("model", model);
        ourModel->Draw(*material->getShader(0));
    }

  private:
    Camera* camera;
    Model* ourModel;
    glm::vec3 lightPos;
};