#pragma once

#include "Plane.h"

#include <glm/glm.hpp>

enum PlaneName { NEAR_PLANE = 0, FAR_PLANE, LEFT_PLANE, RIGHT_PLANE, TOP_PLANE, BOTTOM_PLANE };

const int PLANE_COUNT = 6;

class Frustum {
  public:
    Frustum();
    ~Frustum();

    // 设置投影数据
    void setProjectionData(const float near, const float far, const float fovy,
                           const float aspectRatio);
    void setCameraData(const glm::vec3 &position, const glm::vec3 &view,
                       const glm::vec3 &up);

    void setCameraPosition(const glm::vec3 &position);
    void setCameraOrientation(const glm::vec3 &view, const glm::vec3 &up,
                              const glm::vec3 &right);

    inline const Plane *getPlanes() {
        if (planesDirtyFlag)
            updatePlanes();
        return planes;
    }
    const float *getPackedPlaneData(); // useful to send to shaders，获取打包后的平面数据

    float fovy; // 垂直视角
    float aspectRatio; // 高宽比
    float near, far; // 远近平面
    glm::vec2 nearDimensions; // 近平面的高宽
    glm::vec2 farDimensions;  // 远平面的高宽

  private:
    // in world coordinates
    glm::vec3 position;  // 摄像机的位置
    glm::vec3 view;      // 视线方向
    glm::vec3 up;        // 上方向
    glm::vec3 right;     // 右方向

    // 平面数据
    float packedPlaneData[24]; // 6 times N.x, N.y, N.z and D

    // near, far, left, right, up, down
    Plane planes[PLANE_COUNT];
    bool planesDirtyFlag;  // 标识平面数据是否需要更新
    void updatePlanes();
};