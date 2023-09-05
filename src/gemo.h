#pragma once
#include "SceneObject.h"
#include "Camera.h"
class gemo : public SceneObject{

public:
    gemo();
    ~gemo();

    virtual void
        initialize(); // 设置窗口参数，添加object，设置其数据和纹理，创建camera，设置GL选项
    virtual void render(int passID); // 渲染内容

    Camera* camera;
    unsigned int VAO, VBO;
    unsigned int rez;
    gemo(Camera* camera, unsigned int VAO, unsigned int VBO);
};

