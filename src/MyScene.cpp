#include "MyScene.h"
#include "FirstTriangle.h"
#include "NanoSuit.h"
#include "SceneObject.h"
#include "SkyBox.h"
#include "Sun.h"
#include "Terrain.h"
#include "WaveObject.h"
#include "gemo.h"

#include <iostream>

void MyScene::initialize()
{
    setWindowProperties("OpenGL Demo", SCR_WIDTH, SCR_HEIGHT);
    // glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // create camera fisrt
    Camera *camera = new Camera(glm::vec3(0.0f, 300.0f, 10.0f));
    addCamera(camera);
    setActiveCamera(0);
    camera->setSpeed(30);
    camera->setProjectionMatrix(camera->Zoom, (float)getWindowWidth() / (float)getWindowHeight(), 1.0f, 2048.0f);
    camera->setPositionAndLookAt(glm::vec3(5.0f, 800.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Sun
    Sun *sun = new Sun(-1.0f, -0.6f, 0.0f);
    addSceneObject(sun);

    // wave
    WaveObject *wave = new WaveObject();
    addSceneObject(wave);

    // triangle
    // FirstTriangle *triangle = new FirstTriangle();
    // addSceneObject(triangle);

    // nanosuit
    // NanoSuit *nanosuit = new NanoSuit();
    // addSceneObject(nanosuit);

    // skybox
    std::vector<std::string> faces = {
        "textures/skybox/right.jpg", "textures/skybox/left.jpg",
        "textures/skybox/top.jpg", "textures/skybox/bottom.jpg",
        "textures/skybox/front.jpg", "textures/skybox/back.jpg"};
    SkyBox *skybox = new SkyBox(faces);
    addSceneObject(skybox);

    // terrain
    Terrain *terrain = new Terrain(sun);
    addSceneObject(terrain);

    // gemo* G = new gemo();
    // addSceneObject(G);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Application::initialize(); // init all objects
    std::cout << "Application initialized.\n";
}

void MyScene::render()
{
    Application::render();
    imguiEmbeded->renderBegin();

    static bool isShowMesh = false;
    static float cameraSpeed = 30.0f;

    static bool fullscreen = false;
    ImGui::Checkbox("Show mesh", &isShowMesh);
    if (isShowMesh)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (ImGui::DragFloat("Camera speed", &cameraSpeed, 0.3f, 1.0f, 300.0f))
    {
        getActiveCamera().setSpeed(cameraSpeed);
    }
    if (ImGui::Checkbox("fullscreen", &fullscreen))
    {
        if (fullscreen)
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 1920,
                                 1080, GLFW_DONT_CARE);
        else
            glfwSetWindowMonitor(window, NULL, 300, 200, 1100,
                                 800, GLFW_DONT_CARE);
    }
    // Scene render
    for (SceneObject *scene : sceneObjects)
    {
        scene->render();
    }
    imguiEmbeded->renderEnd();
}
