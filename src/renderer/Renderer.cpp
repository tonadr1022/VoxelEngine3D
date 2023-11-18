//
// Created by Tony Adriansen on 11/16/23.
//

#include "Renderer.h"

Renderer::Renderer(GLFWwindow *window, Camera &camera) : window(window), camera(camera) {

}

void Renderer::render() {
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
}
