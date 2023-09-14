#include "SkyBox.h"
#include "Application.h"
#include "Material.h"
#include "Shader.h"
#include "TextureCube.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

#include <vector>

SkyBox::SkyBox(const std::vector<std::string>& faces)
{
    this->faces = faces;
}

SkyBox::~SkyBox() {}

void SkyBox::initialize()
{

    Shader*   skyboxShader = new Shader("shaders/skybox.vert", "shaders/skybox.frag");
    Material* material     = new Material();
    setMaterial(material);
    material->setShader(0, skyboxShader);
    material->setActiveShader(0);
    material->bind();

    camera = &application->getActiveCamera();

    float skyboxVertices[] = {
        // positions
        -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

        -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
        -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

        1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,
    };

    // skybox VAO

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    TextureCube* texCube = new TextureCube(true);
    texCube->initFromFile(faces);
    material->setTexture(0, texCube);
    material->getActiveShader()->setInt("skybox", 0);
}

void SkyBox::render(int passID)
{

    glDepthFunc(GL_LEQUAL);
    material->bind();
    material->getActiveShader()->setMat4("view", glm::mat4(glm::mat3(camera->getViewMatrix())));
    material->getActiveShader()->setMat4("projection", camera->getProjectionMatrix());
    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);  // set depth function back to default
}