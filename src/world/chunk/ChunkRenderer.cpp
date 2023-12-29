//
// Created by Tony Adriansen on 11/16/23.
//

#include "ChunkRenderer.hpp"

#include "../../Config.hpp"
#include "../../resources/ResourceManager.hpp"
#include "../../shaders/ShaderManager.hpp"

ChunkRenderer::ChunkRenderer(Camera &camera)
    : camera(camera),
      shader(ShaderManager::getShader("chunk")),
      textureAtlasID(ResourceManager::getTexture("texture_atlas")) {}

void ChunkRenderer::render(Chunk &chunk) {
  glm::mat4 model =
      glm::translate(glm::mat4(1.0f), glm::vec3(chunk.m_worldPos, 0.0f));
  shader->setMat4("u_Model", model);
  ChunkMesh &mesh = chunk.getMesh();
  if (mesh.needsUpdate) {
    updateGPUResources(mesh);
    mesh.needsUpdate = false;
  } else if (!mesh.isBuffered) {
    createGPUResources(chunk);
    mesh.isBuffered = true;
  }
  glBindVertexArray(mesh.VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glDrawElements(GL_TRIANGLES, static_cast<GLint>(mesh.indices.size()),
                 GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    std::cerr << "OpenGL Error: " << error << std::endl;
  }
}

void ChunkRenderer::createGPUResources(Chunk &chunk) {
  ChunkMesh &mesh = chunk.getMesh();

  glGenVertexArrays(1, &mesh.VAO);
  glBindVertexArray(mesh.VAO);

  glGenBuffers(1, &mesh.VBO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(ChunkVertex),
               &mesh.vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &mesh.EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0],
               GL_STATIC_DRAW);

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex),
                        reinterpret_cast<void *>(offsetof(ChunkVertex, position)));
  glEnableVertexAttribArray(0);

  // texCoords
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex),
                        reinterpret_cast<void *>(offsetof(ChunkVertex, texCoords)));
  glEnableVertexAttribArray(1);

  // occlusionLevel
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex),
                        reinterpret_cast<void *>(offsetof(ChunkVertex, occlusionLevel)));
  glEnableVertexAttribArray(2);

  // texIndex
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex),
                        reinterpret_cast<void *>(offsetof(ChunkVertex, texIndex)));
  glEnableVertexAttribArray(3);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChunkRenderer::start() {
  updateShaderUniforms();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, textureAtlasID);
}

void ChunkRenderer::updateShaderUniforms() {
  shader->use();
  shader->setBool("u_UseAmbientOcclusion", Config::getUseAmbientOcclusion());
  shader->setInt("u_Texture", 0);
  shader->setMat4("u_Projection", camera.getProjectionMatrix());
  shader->setMat4("u_View", camera.getViewMatrix());
}

void ChunkRenderer::updateGPUResources(ChunkMesh &mesh) {
  glBindVertexArray(mesh.VAO);

  glDeleteBuffers(1, &mesh.VBO);
  glDeleteBuffers(1, &mesh.EBO);

  GLuint nVBO;
  glGenBuffers(1, &nVBO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(ChunkVertex),
               &mesh.vertices[0], GL_STATIC_DRAW);

  GLuint nEBO;
  glGenBuffers(1, &nEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0],
               GL_STATIC_DRAW);

  mesh.VBO = nVBO;
  mesh.EBO = nEBO;

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex),
                        reinterpret_cast<void *>(offsetof(ChunkVertex, position)));
  glEnableVertexAttribArray(0);

  // texCoords
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex),
                        reinterpret_cast<void *>(offsetof(ChunkVertex, texCoords)));
  glEnableVertexAttribArray(1);

  // occlusionLevel
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex),
                        reinterpret_cast<void *>(offsetof(ChunkVertex, occlusionLevel)));
  glEnableVertexAttribArray(2);

  // texIndex
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex),
                        reinterpret_cast<void *>(offsetof(ChunkVertex, texIndex)));
  glEnableVertexAttribArray(3);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

ChunkRenderer::~ChunkRenderer() = default;
