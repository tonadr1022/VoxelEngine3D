//
// Created by Tony Adriansen on 12/3/23.
//

#include "../shaders/ShaderManager.hpp"
#include "BlockBreakRenderer.hpp"
#include "../AppConstants.hpp"
#include "../resources/ResourceManager.hpp"

void BlockBreakRenderer::render(glm::vec3 blockPosition,
                                const Camera &camera,
                                int breakStage) const {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayId);
  const int textureIndex = breakStage;

  auto model = glm::mat4(1.0f);
  model = translate(model, blockPosition);
  model = scale(model, glm::vec3(1.005f));
  model = translate(model, glm::vec3(-0.0025f));

  const Shader *blockBreakShader = ShaderManager::getShader("blockBreak");
  blockBreakShader->use();
  blockBreakShader->setMat4("u_Model", model);
  blockBreakShader->setMat4("u_View", camera.getViewMatrix());
  blockBreakShader->setMat4("u_Projection", camera.getProjectionMatrix());
  blockBreakShader->setInt("u_Texture", 0);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  blockBreakShader->setInt("u_TexIndex", textureIndex);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

BlockBreakRenderer::BlockBreakRenderer() {
  textureArrayId = ResourceManager::getTexture("block_break_array");

  const std::array<float, 120> vertices = {
      // front face
      1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
      1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

      // back face
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
      0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 1.0f, 1.0f, 1.0f,

      // left face
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 1.0f, 1.0f,

      // right face
      0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

      // top face
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

      // bottom face
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
  };

  const std::array<unsigned int, 36> indices = {
      0, 1, 2, 2, 1, 3, // front face
      4, 5, 6, 6, 5, 7, // back face
      8, 9, 10, 10, 9, 11, // left face
      12, 13, 14, 14, 13, 15, // right face
      16, 17, 18, 18, 17, 19, // top face
      20, 21, 22, 22, 21, 23, // bottom face
  };

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

  int stride = 5 * sizeof(float);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *) nullptr);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void *) (3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}
