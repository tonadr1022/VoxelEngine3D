//
// Created by Tony Adriansen on 12/4/23.
//

#include "DebugGui.h"
#include "../world/block/BlockDB.h"

DebugGui::DebugGui(GLFWwindow *window, const char* glsl_version, std::shared_ptr<World> world) : window(window), world(world) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void DebugGui::render() {
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugGui::update() {
// Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuiIO &io = ImGui::GetIO();
    {
        ImGui::Begin("Debug");
        ImGui::Text("Framerate: %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate,
                    io.Framerate);
        glm::vec3 &cameraPos = world->player.getPosition();
        ImGui::Text("Camera Position  %.2f x  %.2f y %.2f z",
                    cameraPos.x, cameraPos.y, cameraPos.z);
        std::string blockName = BlockDB::getBlockData(world->player.inventory.getHeldBlock()).name;
        ImGui::SliderInt("Render Distance", &world->renderDistance, 1, 32);
        ImGui::Text("Block Type: %s", blockName.c_str());
        ImGui::Text("Block ID: %d", world->player.inventory.getHeldBlock());
        ImGui::End();
    }
}

void DebugGui::destroy() {

}
