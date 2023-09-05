#pragma once
#include "EventListener.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class Application;
class ImguiEmbeded : public EventListener {

  public:
    ImguiEmbeded();
    virtual ~ImguiEmbeded();

    virtual void initialize(GLFWwindow *window);
    virtual void render();
    void renderBegin();
    void renderEnd();
    virtual void update(Application *app);
};