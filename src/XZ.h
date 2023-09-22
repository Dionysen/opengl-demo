#include "Application.h"
#include "Camera.h"
#include "Material.h"
#include "SceneObject.h"
#include "Shader.h"
#include "imgui.h"

class XZ : public SceneObject
{
  public:
    XZ() {}
    ~XZ() {}
    virtual void initialize()
    {
        camera = &application->getActiveCamera();

        material = new Material();
        setMaterial(material);

        Shader* shader = new Shader("shaders/axis.vert", "shaders/axis.frag");
        material->setShader(0, shader);
        material->setActiveShader(0);
        shader->use();
        shader->setVec3("color", 1.0f, 1.0f, 1.0f);

        gridWidth = 1.0f;  // 网格线宽度
        gridSize  = 10;    // 网格大小

        generateGridVAO(gridWidth, gridSize);
    }
    virtual void render(int passID = 0)
    {
        material->bind();
        glm::mat4 projection =
            glm::perspective(glm::radians(camera->Zoom), (float)application->getWindowWidth() / (float)application->getWindowHeight(), 0.01f, 100.0f);
        material->getActiveShader()->setMat4("mvp", projection * camera->getViewMatrix() * transform.getTransformationMatrix());

        material->getActiveShader()->setVec3("cameraPos", camera->getPosition());
        material->getActiveShader()->setMat4("model", transform.getTransformationMatrix());

        if (ImGui::CollapsingHeader("Grid"))
        {
            if (ImGui::SliderInt("Grid number", &gridSize, 5, 50))
                generateGridVAO(gridWidth, gridSize);
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
    }
    virtual void update(double deltaTime) {}

    void generateGridVAO(int gridWidth, int gridSize)
    {
        for (int i = -gridSize; i <= gridSize; ++i)
        {
            // 横向线
            vertices.push_back(static_cast<float>(i));
            vertices.push_back(0.0f);
            vertices.push_back(static_cast<float>(gridSize) * gridWidth);

            vertices.push_back(static_cast<float>(i));
            vertices.push_back(0.0f);
            vertices.push_back(-static_cast<float>(gridSize) * gridWidth);

            // 纵向线
            vertices.push_back(static_cast<float>(gridSize) * gridWidth);
            vertices.push_back(0.0f);
            vertices.push_back(static_cast<float>(i));

            vertices.push_back(-static_cast<float>(gridSize) * gridWidth);
            vertices.push_back(0.0f);
            vertices.push_back(static_cast<float>(i));
        }
        // 创建和绑定顶点缓冲对象（VBO）

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // 配置顶点属性指针
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
        glEnableVertexAttribArray(0);
    }

  private:
    GLuint             VAO, VBO;
    std::vector<float> vertices;
    Camera*            camera;
    int                gridSize;
    float              gridWidth;
};