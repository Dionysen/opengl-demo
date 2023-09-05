#include "Application.h"

#include "Camera.h"
#include <iostream>
#include <sstream>

// GLOBAL APPLICATION REFERENCE
Application *app;

void APIENTRY GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    fprintf(stderr, "OpenGL Error: %s\n", message);
}

// GLOBAL CALLBACKS FOR GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
    app->keyCallback(key, scancode, action, mods);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    app->cursorPositionCallback(xpos, ypos);
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
    app->mouseButtonCallback(button, action, mods);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    app->windowResizedCallback(width, height);
}

Application::Application()
    : SCR_WIDTH(1280), SCR_HEIGHT(900), windowTitle("OpenGL App"),
      isCursorEnabled(true), initialized(false), isFullScreen(false),
      activeCamera(0), deltaTime(0.0f), lastFrame(0.0f) {
    app = this;
}

Application::~Application() {
    std::cout << "Application closed.\n";
    std::cout << "Cleaning resources ...\n";
    for (SceneObject *object : sceneObjects) {
        delete object;
    }
    for (Camera *c : cameras) {
        delete c;
    }
    cameras.clear();
    sceneObjects.clear();
    eventListeners.clear();
    glfwTerminate();
    std::cout << "The Process ends.\n";
}

bool Application::initGLContext(int samples, bool srgbFrameBuffer) {
    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, windowTitle.c_str(), NULL,
                              NULL);
    if (!window) {
        std::cout << "Error creating window.";
        return false;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // GLEW
    glewExperimental = GL_TRUE;
    unsigned int err = glewInit();
    if (GLEW_OK != err) {
        std::cout << "Error initing Glew: " << glewGetErrorString(err);
        return false;
    }
    glGetError();
    glEnable(GL_DEPTH_TEST);

    glDebugMessageCallback(GLDebugCallback, nullptr);
    glEnable(GL_DEBUG_OUTPUT);

    const GLubyte* glVersion = glGetString(GL_VERSION);

    if (glVersion) {
        std::cout << "OpenGL版本：" << glVersion << std::endl;
    }
    else {
        std::cerr << "无法获取OpenGL版本信息" << std::endl;
    }
    // set imgui
    this->imguiEmbeded = new ImguiEmbeded();
    this->imguiEmbeded->initialize(window);
    return true;
}

void Application::initialize() {
    for (SceneObject *object : sceneObjects) {
        object->initialize();
    }
    initialized = true;
}

void Application::startLoop() {
    std::stringstream title;
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        if (currentFrame - lastFrame > 1.0) {
            title.str("");
            title << windowTitle << " [FPS: " << (1.0 / deltaTime)
                  << "] [Frame Time: " << deltaTime << "]";
            glfwSetWindowTitle(window, title.str().c_str());

            lastFrame = currentFrame;
        }

        glfwPollEvents();
        update(deltaTime);
        render();

        glfwSwapBuffers(window);
    }

    glfwTerminate();
}

void Application::render() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Application::update(double deltaTime) {}

int Application::addSceneObject(SceneObject *sceneObject) {
    sceneObject->setApplication(this);
    sceneObjects.push_back(sceneObject);
    return sceneObjects.size() - 1;
}

int Application::addEventListener(EventListener *eventListener) {
    eventListeners.push_back(eventListener);
    return eventListeners.size() - 1;
}

void Application::addCamera(Camera *camera) {
    camera->setApplication(this);
    cameras.push_back(camera);
}

void Application::processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (cameras[activeCamera]->isCursorEnabled)
        return;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameras[activeCamera]->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameras[activeCamera]->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameras[activeCamera]->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameras[activeCamera]->ProcessKeyboard(RIGHT, deltaTime);

    // Height
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameras[activeCamera]->ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        cameras[activeCamera]->ProcessKeyboard(DOWN, deltaTime);

    // Faster
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameras[activeCamera]->ProcessKeyboard(FASTER_FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameras[activeCamera]->ProcessKeyboard(FASTER_BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameras[activeCamera]->ProcessKeyboard(FASTER_LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameras[activeCamera]->ProcessKeyboard(FASTER_RIGHT, deltaTime);
}

void Application::windowResizedCallback(const int width, const int height) {
    if (width > 0 && height > 0) {
        SCR_WIDTH = width;
        SCR_HEIGHT = height;

        for (Camera *c : cameras) {
            c->windowResizedCallback(width, height);
        }
        for (EventListener *el : eventListeners) {
            el->windowResizedCallback(width, height);
        }
    }
}

void Application::keyCallback(const int key, const int scanCode,
                              const int action, const int mods) {
    if (key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (cameras[activeCamera]->isCursorEnabled) {
            cameras[activeCamera]->isCursorEnabled = false;
        } else {
            cameras[activeCamera]->isCursorEnabled = true;
        }
        if (!cameras[activeCamera]->isCursorEnabled)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    } else if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
        if (isFullScreen) {
            isFullScreen = false;
            glfwSetWindowMonitor(window, nullptr, 0, 0, SCR_WIDTH, SCR_HEIGHT,
                                 GLFW_DONT_CARE);
        } else {
            isFullScreen = true;
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 1920,
                                 1080, GLFW_DONT_CARE);
        }
    }
    cameras[activeCamera]->keyCallback(key, scanCode, action, mods);

    for (EventListener *el : eventListeners) {
        el->keyCallback(key, scanCode, action, mods);
    }
}

void Application::cursorPositionCallback(const double xpos, const double ypos) {
    cameras[activeCamera]->cursorPositionCallback(xpos, ypos);
    for (EventListener *el : eventListeners) {
        el->cursorPositionCallback(xpos, ypos);
    }
}

void Application::mouseButtonCallback(const int button, const int action,
                                      const int mods) {
    cameras[activeCamera]->mouseButtonCallback(button, action, mods);
    for (EventListener *el : eventListeners) {
        el->mouseButtonCallback(button, action, mods);
    }
}

void Application::setWindowProperties(const std::string &title, const int width,
                                      const int height) {
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    windowTitle = title;
    glfwSetWindowSize(window, width, height);
    glfwSetWindowTitle(window, title.c_str());

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}