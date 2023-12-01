//
// Created by Tony Adriansen on 11/16/23.
//

#include "Renderer.h"
#include "../shaders/ShaderManager.h"

const std::array<int, 120> cubeVertices = {
        // front face
        1, 0, 0, 0, 0,
        1, 1, 0, 1, 0,
        1, 0, 1, 0, 1,
        1, 1, 1, 1, 1,

        // back face
        0, 0, 0, 0, 0,
        0, 0, 1, 0, 1,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 1,

        // left face
        0, 0, 0, 0, 0,
        1, 0, 0, 1, 0,
        0, 0, 1, 0, 1,
        1, 0, 1, 1, 1,

        // right face
        0, 1, 0, 0, 0,
        0, 1, 1, 0, 1,
        1, 1, 0, 1, 0,
        1, 1, 1, 1, 1,

        // top face
        0, 0, 1, 0, 0,
        1, 0, 1, 1, 0,
        0, 1, 1, 0, 1,
        1, 1, 1, 1, 1,

        // bottom face
        0, 0, 0, 0, 0,
        0, 1, 0, 0, 1,
        1, 0, 0, 1, 0,
        1, 1, 0, 1, 1,
};

const std::array<unsigned int, 36> cubeIndices = {
        // front face
        0, 1, 2,
        1, 2, 3,

        // back face
        4, 5, 6,
        5, 6, 7,

        // left face
        8, 9, 10,
        9, 10, 11,

        // right face
        12, 13, 14,
        13, 14, 15,

        // top face
        16, 17, 18,
        17, 18, 19,

        // bottom face
        20, 21, 22,
        21, 22, 23,
};

Renderer::Renderer(GLFWwindow *window, Camera &camera) : window(window), camera(camera),
                                                         highlightShader(ShaderManager::getShader(
                                                                 "highlight")) {
}

void Renderer::render() {
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
}

void Renderer::highlightRender(glm::ivec3 blockPosition) {

//    // create buffers/arrays
//    unsigned int VBO, VAO, EBO;
//    glGenVertexArrays(1, &VAO);
//    glGenBuffers(1, &VBO);
//    glBindVertexArray(VAO);
//    glBindBuffer(1, VBO);
//    glBufferData(1, sizeof(cubeVertices), cubeVertices.data(), GL_STATIC_DRAW);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices.data(), GL_STATIC_DRAW);
//
//    glGenBuffers(1, &EBO);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices.data(), GL_STATIC_DRAW);
//
//    glVertexAttribPointer(0, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(uint32_t), (void *) nullptr);
//    glEnableVertexAttribArray(0);
//
//
//    // render the mesh
//    glm::mat4 model = glm::mat4(1.0f);
//    highlightShader->use();
//    highlightShader->setMat4("u_Model", model);

}

void Renderer::startRender(const std::shared_ptr<Shader>& shader) {
    shader->use();
    shader->setMat4("u_View", camera.getViewMatrix());
    shader->setMat4("u_Projection", camera.getProjectionMatrix());
}

void Renderer::renderCrossHair() {
    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    ShaderManager::getShader("crosshair")->use();
    glBindVertexArray(crossHair.VAO);
    glDrawArrays(GL_LINES, 0, 4); // 4 vertices for crosshair
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

}
