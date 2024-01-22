//
// Created by Tony Adriansen on 11/16/23.
//

#include "ChunkRenderer.hpp"

#include "../../Config.hpp"
#include "../../resources/ResourceManager.hpp"
#include "../../shaders/ShaderManager.hpp"

ChunkRenderer::ChunkRenderer() {
  textureAtlasID = ResourceManager::getTexture("texture_atlas");
}

ChunkRenderer::~ChunkRenderer() = default;

void ChunkRenderer::render(ChunkMesh &mesh, const glm::ivec3 &worldPos, float firstBufferTime) {
  glm::mat4 model = glm::translate(glm::mat4(1.0f), (glm::vec3) worldPos);
  const Shader *shader = ShaderManager::getShader("chunk");
  shader->setMat4("u_Model", model);
  shader->setIVec2("u_ChunkWorldPos", worldPos);
  shader->setFloat("u_FirstBufferTime", firstBufferTime);
  shader->setFloat("u_Time", static_cast<float>(glfwGetTime()));
//  std::cout << "first buffer time: " << firstBufferTime << ", time: " << static_cast<float>(glfwGetTime()) << ", diff: " <<
//  static_cast<float>(glfwGetTime()) - firstBufferTime << std::endl;
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

  //GLenum error = glGetError();
  //if (error != GL_NO_ERROR) {
  //  std::cerr << "OpenGL Error: " << error << std::endl;
  //}
}

void ChunkRenderer::createGPUResources(ChunkMesh &mesh) {
  if (mesh.vertices.empty()) return;
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

  //  must be IPointer
  glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(ChunkVertex), (void *) offsetof(ChunkVertex, vertexData1));
  glEnableVertexAttribArray(0);
  glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(ChunkVertex), (void *) offsetof(ChunkVertex, vertexData2));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void ChunkRenderer::start(const Camera &camera, float worldLightLevel) {
  updateShaderUniforms(camera, worldLightLevel);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, textureAtlasID);
  Config::useWireFrame ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ChunkRenderer::updateShaderUniforms(const Camera &camera, float worldLightLevel) {
  const Shader *shader = ShaderManager::getShader("chunk");
  shader->use();
  shader->setFloat("u_WorldLightLevel", worldLightLevel);
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
  glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(ChunkVertex), &mesh.vertices[0], GL_STATIC_DRAW);

  GLuint nEBO;
  glGenBuffers(1, &nEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

  mesh.VBO = nVBO;
  mesh.EBO = nEBO;

  //  must be IPointer
  glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(ChunkVertex), (void *) offsetof(ChunkVertex, vertexData1));
  glEnableVertexAttribArray(0);
  glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(ChunkVertex), (void *) offsetof(ChunkVertex, vertexData2));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

}

