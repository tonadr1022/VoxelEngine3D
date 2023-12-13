//
// Created by Tony Adriansen on 11/16/23.
//

#include "Renderer.hpp"
#include "../shaders/ShaderManager.hpp"


Renderer::Renderer(GLFWwindow *window) : window(window) {
}

void Renderer::renderBlockOutline(Camera &camera, glm::ivec3 blockPosition) {
    m_blockOutlineRenderer.render(blockPosition, camera);
}

void Renderer::renderBlockBreak(Camera &camera, glm::ivec3 blockPosition, int breakStage) {
    m_blockBreakRenderer.render(blockPosition, camera, breakStage);
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


