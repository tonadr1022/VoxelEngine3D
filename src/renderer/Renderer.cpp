//
// Created by Tony Adriansen on 11/16/23.
//

#include "Renderer.h"
#include "../shaders/ShaderManager.h"


Renderer::Renderer(GLFWwindow *window, Camera &camera) : window(window), camera(camera) {
}

void Renderer::render() {
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
}

void Renderer::renderBlockOutline(glm::ivec3 blockPosition) {
    blockOutlineRenderer.render(blockPosition, camera);
}

void Renderer::renderBlockBreak(glm::ivec3 blockPosition, float breakPercentage) {
    blockBreakRenderer.render(blockPosition, camera, breakPercentage);
}

void Renderer::startRender(const std::shared_ptr<Shader> &shader) {
    shader->use();
    shader->setMat4("u_View", camera.getViewMatrix());
    shader->setMat4("u_Projection", camera.getProjectionMatrix());
}

void Renderer::renderCrossHair() const {
    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    ShaderManager::getShader("crosshair")->use();
    glBindVertexArray(crossHair.VAO);
    glDrawArrays(GL_LINES, 0, 4); // 4 vertices for crosshair
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
}


