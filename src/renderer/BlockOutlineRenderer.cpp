//
// Created by Tony Adriansen on 12/2/23.
//

#include "../shaders/ShaderManager.hpp"
#include "BlockOutlineRenderer.hpp"

const std::array<float, 24> cubeVertices = {
    0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
};

const std::array<unsigned int, 24> cubeIndices = {
    0, 1, 1, 2, 2, 3, 3, 0,
    4, 5, 5, 6, 6, 7, 7, 4,
    0, 4, 1, 5, 2, 6, 3, 7
};

BlockOutlineRenderer::BlockOutlineRenderer() : VAO(0), VBO(0), EBO(0) {
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               cubeVertices.size() * sizeof(unsigned int),
               cubeVertices.data(),
               GL_STATIC_DRAW);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               cubeIndices.size() * sizeof(unsigned int),
               cubeIndices.data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        3 * sizeof(float),
                        (void *) nullptr);
  glEnableVertexAttribArray(0);
}

void BlockOutlineRenderer::render(glm::vec3 blockPosition,
                                  Camera &camera) const {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, blockPosition);
  model = glm::scale(model, glm::vec3(1.005f));
  model = glm::translate(model, glm::vec3(-0.0025f));

  std::shared_ptr<Shader> outlineShader = ShaderManager::getShader("outline");
  outlineShader->use();
  outlineShader->setMat4("u_Model", model);
  outlineShader->setMat4("u_View", camera.getViewMatrix());
  outlineShader->setMat4("u_Projection", camera.getProjectionMatrix());
  outlineShader->setFloat("u_LineWidth", 0.01f);

  // set width using opengl command
  glBindVertexArray(VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glDrawElements(GL_LINES, cubeIndices.size(), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
