//
// Created by Tony Adriansen on 12/4/23.
//

#include "DebugGui.hpp"
#include "../world/block/BlockDB.hpp"
#include "../Config.hpp"

DebugGui::DebugGui(GLFWwindow *context, const char *glsl_version) : context(context) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(context, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void DebugGui::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void DebugGui::endFrame() {
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(context, &display_w, &display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

DebugGui::~DebugGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
