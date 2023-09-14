#include "ImguiEmbeded.h"
#include <iostream>

ImguiEmbeded::ImguiEmbeded() {}

ImguiEmbeded::~ImguiEmbeded() {}

void ImguiEmbeded::initialize(GLFWwindow* window)
{  // Init Dear Imgui
    const char* glsl_version = "#version 330";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void ImguiEmbeded::render()  // default render
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        std::cout << "IMDUI\n";
        // setup
        ImGui::Begin("Dear ImGui");
        // ...
        ImGui::Text("Hello");

        // Frame rate
        ImGui::Text(
            "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
            ImGui::GetIO().Framerate);
        ImGui::End();
        // ImGui::ShowDemoWindow();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImguiEmbeded::renderBegin()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        // setup
        ImGui::Begin("OpenGL Demo");
        ImGui::Text("Welcome to my Scene!");
        ImGui::Text("Press \"Alt + C\" to visiable cursor or not.");
        // Frame rate
        ImGui::Text(
            "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
            ImGui::GetIO().Framerate);
    }
}

void ImguiEmbeded::renderEnd()
{
    {
        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImguiEmbeded::update(Application* app) {}
