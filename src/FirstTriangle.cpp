#include "FirstTriangle.h"
#include "Application.h"
#include "Texture2D.h"
#include <iostream>

FirstTriangle::FirstTriangle() {}

FirstTriangle::~FirstTriangle() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

void FirstTriangle::initialize() {

    Material *material = new Material();
    setMaterial(material);

    Shader *shader = new Shader("shaders/lamp.vert", "shaders/lamp.frag");
    material->setShader(0, shader);
    material->setActiveShader(0);

    // 创建纹理，并设置要绑定的纹理单元
    Texture2D *texture = new Texture2D();
    texture->setMipmapLevels(1);
    texture->initFromFile("textures/wood.png", ImageFormat::IMAGE_FORMAT_SRGB8);
    texture->bind();
    texture->changeTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    texture->changeTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    texture->changeTextureParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    texture->changeTextureParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Texture2D* texture2 = new Texture2D();
    texture2->setMipmapLevels(1);
    texture2->initFromFile("textures/wall.jpg", ImageFormat::IMAGE_FORMAT_SRGB8);
    texture2->bind();
    texture2->changeTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    texture2->changeTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    texture2->changeTextureParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    texture2->changeTextureParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    material->setTexture(0, texture);
    material->setTexture(1, texture2);
    material->bind();
    
    // 绑定采样器
    material->useShader();
    material->getActiveShader()->setInt("texture1", 0);
    material->getActiveShader()->setInt("texture2", 1);

    camera = &application->getActiveCamera();

    float vertices[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void FirstTriangle::render(int passID) {
    getMaterial().useShader();
    glm::mat4 projection =
        glm::perspective(glm::radians(camera->Zoom),
                         (float)application->getWindowWidth() /
                             (float)application->getWindowHeight(),
                         0.1f, 1000.0f);
    glm::mat4 view = camera->getViewMatrix();

    glBindVertexArray(VAO);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(20.0f));

    static glm::vec3 pos = glm::vec3(0.0f);
    //ImGui::SliderFloat3("x", glm::value_ptr(pos), -30.0f, 30.0f,"%.3f");
    //ImGui::DragFloat3("x", glm::value_ptr(pos), 0.2f, -30.0f, 30.0f, "%.3f");
    model = glm::translate(model, pos);

    getMaterial().getShader(0)->setMat4("projection", camera->getProjectionMatrix());
    getMaterial().getShader(0)->setMat4("view", view);
    getMaterial().getShader(0)->setMat4("model", model);

    material->bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
