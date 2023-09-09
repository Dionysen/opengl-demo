#include "Camera.h"
#include <GLFW/glfw3.h>
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY), Zoom(ZOOM), isCursorEnabled(false)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;

    transform.setPosition(position, WORLD);

    frustum.setProjectionData(0.1f, 100.0f, 45.0f, 1.0f);
    frustum.setCameraData(transform.getPosition(), -transform.getLocalZVector(),
                          transform.getLocalYVector());

    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
               float upZ, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY), Zoom(ZOOM), isCursorEnabled(false)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;

    frustum.setProjectionData(0.1f, 2048.0f, ZOOM, (float)application->getWindowWidth() / (float)application->getWindowHeight());
    frustum.setCameraData(transform.getPosition(), -transform.getLocalZVector(),
                          transform.getLocalYVector());

    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() // 生成观察矩阵
{
    return glm::lookAt(transform.getPosition(), transform.getPosition() + Front, Up);
}

glm::mat4 Camera::getProjectionMatrix()
{
    if (projectionMatrixDirtyFlag)
    {
        projectionMatrix = glm::perspective(glm::radians(frustum.fovy), frustum.aspectRatio, frustum.near, frustum.far);
        projectionMatrixDirtyFlag = false;
    }
    return projectionMatrix;
}

void Camera::setProjectionMatrix(float fovy, float aspectRatio, float near, float far)
{
    frustum.setProjectionData(near, far, fovy, aspectRatio);
    projectionMatrixDirtyFlag = true;
}

void Camera::setPositionAndLookAt(const glm::vec3 &position, const glm::vec3 &lookAt, const glm::vec3 &upVector)
{
    Up = upVector;
    transform.setPosition(position, WORLD);
    this->lookAt.x = lookAt.x;
    this->lookAt.y = lookAt.y;
    this->lookAt.z = lookAt.z;
    updateInternals();
}

void Camera::setPosition(float x, float y, float z)
{
    transform.setPosition(x, y, z, WORLD);
    updateInternals();
}

void Camera::setLookAt(float x, float y, float z)
{
    lookAt.x = x;
    lookAt.y = y;
    lookAt.z = z;

    updateInternals();
}

void Camera::updateInternals()
{
    glm::mat4 lookat = glm::lookAt(transform.getPosition(), lookAt, Up);
    transform.setLocalXVector(lookat[0].x, lookat[1].x, lookat[2].x);
    transform.setLocalYVector(lookat[0].y, lookat[1].y, lookat[2].y);
    transform.setLocalZVector(lookat[0].z, lookat[1].z, lookat[2].z);

    frustum.setCameraPosition(transform.getPosition());
    frustum.setCameraOrientation(-transform.getLocalZVector(), transform.getLocalYVector(), transform.getLocalXVector());
}

// callback
void Camera::windowResizedCallback(const int width, const int height)
{
    setProjectionMatrix(frustum.fovy, (float)width / (float)height, frustum.near, frustum.far);
    glViewport(0, 0, width, height);
}

void Camera::keyCallback(const int key, const int scanCode, const int action,
                         const int mods) {}

void Camera::cursorPositionCallback(const double xpos, const double ypos)
{
    if (isCursorEnabled)
        return;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset =
        lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    xoffset *= MouseSensitivity; // x方向的鼠标偏离
    yoffset *= MouseSensitivity; // y方向的鼠标偏离

    Yaw += xoffset;   // 偏航
    Pitch += yoffset; // 仰角

    if (true) // 确保仰角足够大时屏幕不会被翻转
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }
    updateCameraVectors();
}

void Camera::mouseScrollCallback(double xoffset, double yoffset)
{
    if (!isCursorEnabled)
        return;
    Zoom -= static_cast<float>(yoffset);
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime; // 设定速度
    // 根据方向调整方向向量
    if (direction == FORWARD)
        transform.setPosition(transform.getPosition() + Front * velocity, WORLD);
    if (direction == BACKWARD)
        transform.setPosition(transform.getPosition() - Front * velocity, WORLD);
    if (direction == LEFT)
        transform.setPosition(transform.getPosition() - Right * velocity, WORLD);
    if (direction == RIGHT)
        transform.setPosition(transform.getPosition() + Right * velocity, WORLD);
    if (direction == UP)
        transform.setPosition(transform.getPosition().x, transform.getPosition().y + velocity, transform.getPosition().z, WORLD);
    if (direction == DOWN)
        transform.setPosition(transform.getPosition().x, transform.getPosition().y - velocity, transform.getPosition().z, WORLD);

    // Position.y = 0.0f; // 确保不会偏离xz平面

    if (direction == FASTER_FORWARD)
        transform.setPosition(transform.getPosition() + Front * (velocity * 10), WORLD);
    if (direction == FASTER_BACKWARD)
        transform.setPosition(transform.getPosition() - Front * (velocity * 10), WORLD);
    if (direction == FASTER_LEFT)
        transform.setPosition(transform.getPosition() - Right * (velocity * 10), WORLD);
    if (direction == FASTER_RIGHT)
        transform.setPosition(transform.getPosition() + Right * (velocity * 10), WORLD);
    if (direction == FASTER_UP)
        transform.setPosition(transform.getPosition().x, transform.getPosition().y + velocity * 10, transform.getPosition().z, WORLD);
    if (direction == FASTER_DOWN)
        transform.setPosition(transform.getPosition().x, transform.getPosition().y - velocity * 10, transform.getPosition().z, WORLD);
}

void Camera::update(double deltaTime) {}

void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front{0};
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // 同时重新计算了右向量和上向量
    Right = glm::normalize(glm::cross(Front, WorldUp));
    // 将向量归一化，因为你向上或向下看的次数越多，它们的长度就越接近0，这会导致移动速度变慢。
    Up = glm::normalize(glm::cross(Right, Front));
}