//
// Created by Tony Adriansen on 2/7/24.
//

#include "ChunkMapRenderer.hpp"
#include "../shaders/Shader.hpp"
#include "../shaders/ShaderManager.hpp"
#include "../resources/ResourceManager.hpp"

ChunkMapRenderer::ChunkMapRenderer(int length)
    : VAO(0), VBO(0), EBO(0), indices_size(0), vertices_size(0), m_length(length) {
  textureAtlasID = ResourceManager::getTexture("texture_atlas");
}
void checkError() {
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    switch (error) {
      case GL_INVALID_OPERATION:std::cerr << "OpenGL Error: GL_INVALID_OPERATION" << std::endl;
        break;
      default:std::cerr << "OpenGL Error: Unknown error" << std::endl;
        break;
    }
  }
}

void ChunkMapRenderer::genBuffer(std::vector<int> &colors, int loadDistance) {

  vertices.clear();
  indices.clear();
  int length = loadDistance * 2 + 1;
  m_length = length;
  for (int j = 0; j < colors.size(); j++) {
    int y = j / length;
    int x = j % length;
    auto xPos = static_cast<float>(x);
    auto yPos = static_cast<float>(y);
    unsigned int indexOffset = vertices.size();
    int colorIndex = colors[j];
    for (int i = 0; i < 4; i++) {
      vertices.push_back({quadVertices[i * 4] + xPos,
                          quadVertices[i * 4 + 1] + yPos,
                          quadVertices[i * 4 + 2],
                          quadVertices[i * 4 + 3],
                          static_cast<unsigned int>(colorIndex)});
    }
    for (unsigned int quadIndex : quadIndices) {
      indices.push_back(quadIndex + indexOffset);
    }
  }

  indices_size = indices.size();
  vertices_size = vertices.size();
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, indices_size * sizeof(QuadVertex), &vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*) offsetof(QuadVertex, x));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*) offsetof(QuadVertex, u));
  glEnableVertexAttribArray(1);
  glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(QuadVertex), (void*) offsetof(QuadVertex, texIndex));
  glEnableVertexAttribArray(2);

  vertices.clear();
  indices.clear();
}


void ChunkMapRenderer::draw(float scale) const {
  const Shader* shader = ShaderManager::getShader("quad");
  shader->use();
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::scale(model, glm::vec3(1.0f/scale));
  model = glm::translate(model, glm::vec3(-scale,  scale-m_length, 0.0f));
  shader->setMat4("u_MVP", model);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, textureAtlasID);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glDrawElements(GL_TRIANGLES, static_cast<GLint>(indices_size), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
