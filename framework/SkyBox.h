#pragma once
#include "Camera.h"
#include "SceneObject.h"
#include <string>
#include <vector>

class SkyBox : public SceneObject {
  public:
    SkyBox(const std::vector<std::string> &faces);
    ~SkyBox(); // 删除内存

    virtual void initialize();
    virtual void render(int passID); // 渲染内容

    unsigned int loadCubemap(std::vector<std::string> faces);

    Camera *camera;
    unsigned int cubemapTexture;
    unsigned int skyboxVAO, skyboxVBO;
    std::vector<std::string> faces;
};