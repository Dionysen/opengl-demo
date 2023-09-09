#include "NanoSuit.h"
#include "Material.h"
#include "Application.h"
#include "Shader.h"
#include <iostream>

NanoSuit::NanoSuit() {}

NanoSuit::~NanoSuit() {}

void NanoSuit::initialize()
{
    std::cout << "Init nanosuit ...\n";
    Material *material = new Material();
    Shader *shader = new Shader("shaders/model.vert", "shaders/model.frag");
    material->setShader(0, shader);
    material->setActiveShader(0);
    material->useShader();

    setMaterial(material);
    material->bind();

    camera = &application->getActiveCamera();

    ourModel = new Model("textures/nanosuit/nanosuit.obj");
    std::cout << "The nanosuit initialized.\n";
}

void NanoSuit::render(int passID)
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