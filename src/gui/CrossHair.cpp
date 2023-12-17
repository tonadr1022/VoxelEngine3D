//
// Created by Tony Adriansen on 11/29/23.
//

#include "CrossHair.hpp"
#include "../shaders/ShaderManager.hpp"

CrossHair::CrossHair() : VAO(0), VBO(0) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    std::array<float, 8> vertices = {
            -0.75f,  0.0f,
            0.75f,  0.0f,
            0.0f,   1.0f,
            0.0f,  -1.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.03f));
    auto crossHairColor = glm::vec3(1.0f);

    ShaderManager::getShader("crosshair")->use();
    ShaderManager::getShader("crosshair")->setMat4("u_Model", model);
    ShaderManager::getShader("crosshair")->setVec3("u_Color", crossHairColor);
}
