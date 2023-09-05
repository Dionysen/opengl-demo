#pragma once
#include "Camera.h"
#include "SceneObject.h"

class FirstTriangle : public SceneObject {

  public:
    FirstTriangle();
    ~FirstTriangle(); // 删除内存

    virtual void
    initialize(); // 设置窗口参数，添加object，设置其数据和纹理，创建camera，设置GL选项
    virtual void render(int passID); // 渲染内容

    glm::vec3 lightPos;

    Camera *camera;
    unsigned int shaderProgram;
    unsigned int VBO, VAO, EBO;
    unsigned int lightVAO;

    unsigned int texture0;
};