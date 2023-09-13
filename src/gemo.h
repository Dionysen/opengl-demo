#pragma once
#include "Camera.h"
#include "SceneObject.h"
#include <Material.h>

// #include "stb_image.h"
#include <Texture.h>

class gemo : public SceneObject
{

  public:
    gemo()
    {}
    ~gemo()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    virtual void initialize()
    {
        material = new Material();
        // Shader* shader = new Shader("shaders/test.vert", "shaders/test.frag",
        // "shaders/test.geom");
        Shader* shader =
            new Shader("shaders/test.vert", "shaders/test.frag", nullptr, "shaders/test.tesc", "shaders/test.tese");
        material->setShader(0, shader);
        material->setActiveShader(0);

        setMaterial(material);
        camera = &application->getActiveCamera();

        GLint maxTessLevel;
        glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
        glEnable(GL_DEPTH_TEST);

        unsigned int texture;
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,
                      texture); // all upcoming GL_TEXTURE_2D operations now have
                                // effect on this texture object
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_REPEAT); // set texture wrapping to GL_REPEAT (default
                                    // wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        // The FileSystem::getPath(...) is part of the GitHub repository so we can
        // find files on any IDE/platform; replace it with your own image path.
        unsigned char* data = stbi_load("textures/river_heightmap.png", &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

        shader->use();
        shader->setInt("heightMap", 0);

        std::vector<float> vertices;

        rez = 40;
        for (unsigned i = 0; i <= rez - 1; i++)
        {
            for (unsigned j = 0; j <= rez - 1; j++)
            {
                vertices.push_back(-width / 2.0f + width * i / (float)rez);   // v.x
                vertices.push_back(0.0f);                                     // v.y
                vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
                vertices.push_back(i / (float)rez);                           // u
                vertices.push_back(j / (float)rez);                           // v

                vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
                vertices.push_back(0.0f);                                         // v.y
                vertices.push_back(-height / 2.0f + height * j / (float)rez);     // v.z
                vertices.push_back((i + 1) / (float)rez);                         // u
                vertices.push_back(j / (float)rez);                               // v

                vertices.push_back(-width / 2.0f + width * i / (float)rez);         // v.x
                vertices.push_back(0.0f);                                           // v.y
                vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
                vertices.push_back(i / (float)rez);                                 // u
                vertices.push_back((j + 1) / (float)rez);                           // v

                vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez);   // v.x
                vertices.push_back(0.0f);                                           // v.y
                vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
                vertices.push_back((i + 1) / (float)rez);                           // u
                vertices.push_back((j + 1) / (float)rez);                           // v
            }
        }
        std::cout << "Loaded " << rez * rez << " patches of 4 control points each" << std::endl;
        std::cout << "Processing " << rez * rez * 4 << " vertices in vertex shader" << std::endl;

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texCoord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);

        glPatchParameteri(GL_PATCH_VERTICES, 4);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    // glPointSize(5.0);}
    virtual void render(int passID)
    {
        material->bind();

        glm::mat4 projection = glm::perspective(
            glm::radians(camera->Zoom), (float)application->getWindowWidth() / (float)application->getWindowHeight(),
            0.1f, 100000.0f);
        material->getActiveShader()->setMat4("projection", projection);
        material->getActiveShader()->setMat4("view", camera->getViewMatrix());

        glm::mat4 model = glm::mat4(1.0f);
        material->getActiveShader()->setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);
    }

    Camera* camera;
    unsigned int VAO, VBO;
    unsigned int rez;
    gemo(Camera* camera, unsigned int VAO, unsigned int VBO) : camera(camera), VAO(VAO), VBO(VBO)
    {}
};
