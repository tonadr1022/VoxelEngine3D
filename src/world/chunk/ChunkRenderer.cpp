//
// Created by Tony Adriansen on 11/16/23.
//

#include "ChunkRenderer.hpp"

#include "../../Config.hpp"
#include "../../resources/ResourceManager.hpp"
#include "../../shaders/ShaderManager.hpp"

ChunkRenderer::ChunkRenderer() {
  shader = ShaderManager::getShader("chunk");
  textureAtlasID = ResourceManager::getTexture("texture_atlas");
}

ChunkRenderer::~ChunkRenderer() = default;

void ChunkRenderer::render(ChunkMesh &mesh, const glm::ivec2 &worldPos) {
  glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(worldPos, 0.0f));
  shader->setMat4("u_Model", model);
  if (mesh.needsUpdate) {
    updateGPUResources(mesh);
    mesh.needsUpdate = false;
  } else if (!mesh.isBuffered) {
    createGPUResources(mesh);
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

void ChunkRenderer::createGPUResources(ChunkMesh &mesh) {
  if (mesh.vertices.empty()) return;
  glGenVertexArrays(1, &mesh.VAO);
  glBindVertexArray(mesh.VAO);

  glGenBuffers(1, &mesh.VBO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(uint32_t),
               &mesh.vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &mesh.EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0],
               GL_STATIC_DRAW);

  // vertex data only attribute, must be IPointer
  glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(uint32_t), (void *) nullptr);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void ChunkRenderer::start(const Camera &camera) {
  updateShaderUniforms(camera);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, textureAtlasID);
}

void ChunkRenderer::updateShaderUniforms(const Camera &camera) {
  shader->use();
  shader->setFloat("u_Time", static_cast<float>(glfwGetTime()));
  shader->setBool("u_UseAmbientOcclusion", Config::getUseAmbientOcclusion());
  shader->setInt("u_Texture", 0);
  shader->setMat4("u_Projection", camera.getProjectionMatrix());
  shader->setMat4("u_View", camera.getViewMatrix());
}

void ChunkRenderer::updateGPUResources(ChunkMesh &mesh) {
  if (mesh.vertices.empty()) return;
  glBindVertexArray(mesh.VAO);

  glDeleteBuffers(1, &mesh.VBO);
  glDeleteBuffers(1, &mesh.EBO);

  GLuint nVBO;
  glGenBuffers(1, &nVBO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(uint32_t),
               &mesh.vertices[0], GL_STATIC_DRAW);

  GLuint nEBO;
  glGenBuffers(1, &nEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0],
               GL_STATIC_DRAW);

  mesh.VBO = nVBO;
  mesh.EBO = nEBO;

  // vertex data only attribute, must be IPointer
  glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(uint32_t), (void *) nullptr);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

}

