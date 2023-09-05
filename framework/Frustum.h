#pragma once

#include "Plane.h"

#include <glm/glm.hpp>

enum PlaneName { NEAR_PLANE = 0, FAR_PLANE, LEFT_PLANE, RIGHT_PLANE, TOP_PLANE, BOTTOM_PLANE };

const int PLANE_COUNT = 6;

class Frustum {
  public:
    Frustum();
    ~Frustum();

    // ����ͶӰ����
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
    const float *getPackedPlaneData(); // useful to send to shaders����ȡ������ƽ������

    float fovy; // ��ֱ�ӽ�
    float aspectRatio; // �߿��
    float near, far; // Զ��ƽ��
    glm::vec2 nearDimensions; // ��ƽ��ĸ߿�
    glm::vec2 farDimensions;  // Զƽ��ĸ߿�

  private:
    // in world coordinates
    glm::vec3 position;  // �������λ��
    glm::vec3 view;      // ���߷���
    glm::vec3 up;        // �Ϸ���
    glm::vec3 right;     // �ҷ���

    // ƽ������
    float packedPlaneData[24]; // 6 times N.x, N.y, N.z and D

    // near, far, left, right, up, down
    Plane planes[PLANE_COUNT];
    bool planesDirtyFlag;  // ��ʶƽ�������Ƿ���Ҫ����
    void updatePlanes();
};