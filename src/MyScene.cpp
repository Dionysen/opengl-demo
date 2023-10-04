#include "MyScene.h"

#include <iostream>

#include "BoneAnimateion.h"
#include "FirstTriangle.h"
#include "NanoSuit.h"
#include "SceneObject.h"
#include "SkyBox.h"
#include "Sun.h"
#include "Terrain.h"
#include "Terrain2.h"
#include "WaveObject.h"
#include "XZ.h"
void MyScene::initialize()
{
    setWindowProperties("OpenGL Demo", SCR_WIDTH, SCR_HEIGHT);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // create camera fisrt
    Camera* camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f));
    addCamera(camera);
    setActiveCamera(0);
    cameraSpeed = 2.5f;
    camera->setSpeed(cameraSpeed);
    camera->setProjectionMatrix(camera->Zoom, (float)getWindowWidth() / (float)getWindowHeight(), 0.01f, 2048.0f);

    // Sun
    // Sun* sun = new Sun(-1.0f, -0.6f, 0.0f);
    // addSceneObject(sun);

    // wave
    // WaveObject *wave = new WaveObject();
    // addSceneObject(wave);

    // triangle
    // FirstTriangle* triangle = new FirstTriangle();
    // addSceneObject(triangle);

    //   nanosuit
    //std::shared_ptr<NanoSuit> nanosuit = std::make_shared<NanoSuit> ();
    //NanoSuit*                 nano     = new NanoSuit();
    //addSceneObject(nano);

    // skybox
    // std::vector<std::string> faces = {
    //     "textures/skybox/right.jpg",  "textures/skybox/left.jpg",  "textures/skybox/top.jpg",
    //     "textures/skybox/bottom.jpg", "textures/skybox/front.jpg", "textures/skybox/back.jpg",
    // };
    // SkyBox* skybox = new SkyBox(faces);
    // addSceneObject(skybox);

    // terrain
    // Terrain* terrain = new Terrain(sun);
    // addSceneObject(terrain);

    // gemo* G = new gemo();
    // addSceneObject(G);

    // BoneAnimation* boneAnim = new BoneAnimation();
    // addSceneObject(boneAnim);

    XZ* xz = new XZ();
    addSceneObject(xz);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    Application::initialize();  // init all objects
    std::cout << "Application initialized.\n";
}

void MyScene::render()
{
    Application::render();
    imguiEmbeded->renderBegin();

    if (ImGui::Checkbox("Show mesh", &isShowMesh))
    {
        if (isShowMesh)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("fullscreen", &isFullScreen))
    {
        if (isFullScreen)
        {
            TEMP_SCR_HEIGHT = SCR_HEIGHT;
            TEMP_SCR_WIDTH  = SCR_WIDTH;
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 1920, 1080, GLFW_DONT_CARE);
        }
        else
        {
            glfwSetWindowMonitor(window, NULL, 200, 100, TEMP_SCR_WIDTH, TEMP_SCR_HEIGHT, GLFW_DONT_CARE);
        }
    }

    if (ImGui::DragFloat("Camera speed", &cameraSpeed, 0.3f, 1.0f, 300.0f))
    {
        getActiveCamera().setSpeed(cameraSpeed);
    }

    // Scene render
    for (SceneObject* scene : sceneObjects) { scene->render(); }
    imguiEmbeded->renderEnd();
}
