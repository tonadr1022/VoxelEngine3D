//
// Created by Tony Adriansen on 11/16/23.
//

#include "Renderer.h"
#include "../shaders/ShaderManager.h"


Renderer::Renderer(GLFWwindow *window) : window(window) {
}

void Renderer::render(Camera &camera) {
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
}

void Renderer::renderBlockOutline(Camera &camera, glm::ivec3 blockPosition) {
    blockOutlineRenderer.render(blockPosition, camera);
}

void Renderer::renderBlockBreak(Camera &camera, glm::ivec3 blockPosition, float breakPercentage) {
    blockBreakRenderer.render(blockPosition, camera, breakPercentage);
}

void Renderer::startRender(Camera &camera, const std::shared_ptr<Shader> &shader) {
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


