#pragma once
#include "SceneObject.h"
#include "Camera.h"
class gemo : public SceneObject{

public:
    gemo();
    ~gemo();

    virtual void
        initialize(); // ���ô��ڲ��������object�����������ݺ���������camera������GLѡ��
    virtual void render(int passID); // ��Ⱦ����

    Camera* camera;
    unsigned int VAO, VBO;
    unsigned int rez;
    gemo(Camera* camera, unsigned int VAO, unsigned int VBO);
};

