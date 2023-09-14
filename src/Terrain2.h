#pragma once
#include "Camera.h"
#include "Material.h"
#include "SceneObject.h"
#include "Texture.h"
#include "Texture2D.h"

class gemo : public SceneObject
{
  public:
    gemo() {}
    ~gemo()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    virtual void initialize()
    {
        camera = &application->getActiveCamera();

        Shader* shader = new Shader("shaders/test.vert", "shaders/test.frag", nullptr, "shaders/test.tesc", "shaders/test.tese");

        GLint maxTessLevel;
        glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);

        Texture2D* tex = new Texture2D();
        tex->initFromFile("textures/river_heightmap.png", ImageFormat::IMAGE_FORMAT_SRGBA8);
        tex->changeTextureParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        tex->changeTextureParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
        tex->changeTextureParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        tex->changeTextureParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        material = new Material();
        setMaterial(material);

        material->setShader(0, shader);
        material->setActiveShader(0);
        material->setTexture(0, tex);
        shader->setInt("heightMap", 0);

        material->bind();

        std::vector<float> vertices;

        int width  = tex->getImageData(0).width;
        int height = tex->getImageData(0).height;

        rez = 40;
        for (unsigned i = 0; i <= rez - 1; i++)
        {
            for (unsigned j = 0; j <= rez - 1; j++)
            {
                vertices.push_back(-width / 2.0f + width * i / (float)rez);          // v.x
                vertices.push_back(0.0f);                                            // v.y
                vertices.push_back(-height / 2.0f + height * j / (float)rez);        // v.z
                vertices.push_back(i / (float)rez);                                  // u
                vertices.push_back(j / (float)rez);                                  // v

                vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez);    // v.x
                vertices.push_back(0.0f);                                            // v.y
                vertices.push_back(-height / 2.0f + height * j / (float)rez);        // v.z
                vertices.push_back((i + 1) / (float)rez);                            // u
                vertices.push_back(j / (float)rez);                                  // v

                vertices.push_back(-width / 2.0f + width * i / (float)rez);          // v.x
                vertices.push_back(0.0f);                                            // v.y
                vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez);  // v.z
                vertices.push_back(i / (float)rez);                                  // u
                vertices.push_back((j + 1) / (float)rez);                            // v

                vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez);    // v.x
                vertices.push_back(0.0f);                                            // v.y
                vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez);  // v.z
                vertices.push_back((i + 1) / (float)rez);                            // u
                vertices.push_back((j + 1) / (float)rez);                            // v
            }
        }
        std::cout << "Loaded " << rez * rez << " patches of 4 control points each" << std::endl;
        std::cout << "Processing " << rez * rez * 4 << " vertices in vertex shader" << std::endl;

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        // position attrssA Wibute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texCoord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);

        glPatchParameteri(GL_PATCH_VERTICES, 4);
    }

    virtual void render(int passID)
    {
        material->bind();
        material->getActiveShader()->setMat4("projection", camera->getProjectionMatrix());
        material->getActiveShader()->setMat4("view", camera->getViewMatrix());

        glm::mat4 model = glm::mat4(1.0f);
        material->getActiveShader()->setMat4("model", transform.getTransformationMatrix());

        glBindVertexArray(VAO);
        glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);
    }

    Camera* camera;

    unsigned int VAO, VBO;
    unsigned int rez;
    gemo(Camera* camera, unsigned int VAO, unsigned int VBO) : camera(camera), VAO(VAO), VBO(VBO) {}
};
