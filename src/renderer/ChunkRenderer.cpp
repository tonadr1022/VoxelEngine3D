//
// Created by Tony Adriansen on 11/16/23.
//

#include "ChunkRenderer.hpp"

#include "../Config.hpp"
#include "../resources/ResourceManager.hpp"
#include "../shaders/ShaderManager.hpp"
#include "../world/World.hpp"

ChunkRenderer::ChunkRenderer() = default;

void ChunkRenderer::init() {
  textureAtlasID = ResourceManager::getTexture("texture_atlas");
  m_waterTexIndex = BlockDB::getTexIndex(Block::WATER, TOP);
}


ChunkRenderer::~ChunkRenderer() = default;

void ChunkRenderer::render(const World& world) const {
  const Camera& camera = world.player.camera;
  startFrame(camera, world.getWorldLightLevel());
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  for (auto& pos : world.getOpaqueRenderSet()) {
    Chunk* chunk = world.getChunkRawPtrOrNull(pos);
    if (!chunk) continue;
    if (!world.m_viewFrustum.isBoxInFrustum(chunk->m_boundingBox)) continue;
    if (chunk->m_firstBufferTime == 0) chunk->m_firstBufferTime = static_cast<float>(glfwGetTime());
    renderChunk(chunk->m_opaqueMesh, camera, chunk->m_worldPos, chunk->m_firstBufferTime);
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  for (auto& pos : world.getTransparentRenderVector()) {
    Chunk* chunk = world.getChunkRawPtrOrNull(pos);
    if (!chunk) continue;
    if (!world.m_viewFrustum.isBoxInFrustum(chunk->m_boundingBox)) continue;
    renderChunk(chunk->m_transparentMesh, camera, chunk->m_worldPos, chunk->m_firstBufferTime);
  }

}

void ChunkRenderer::renderChunk(ChunkMesh& mesh,
                                const Camera& camera,
                                const glm::ivec3& worldPos,
                                float firstBufferTime) const {
  glm::mat4 model = glm::translate(glm::mat4(1.0f), (glm::vec3) worldPos);
  const Shader* shader = ShaderManager::getShader("chunk");
  shader->setMat4("u_MVP", camera.getVPMatrix() * model);
  shader->setIVec2("u_ChunkWorldPos", worldPos);
  shader->setFloat("u_FirstBufferTime", firstBufferTime);
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
}

void ChunkRenderer::createGPUResources(ChunkMesh& mesh) {
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
  glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(ChunkVertex), (void*) offsetof(ChunkVertex, vertexData1));
  glEnableVertexAttribArray(0);
  glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(ChunkVertex), (void*) offsetof(ChunkVertex, vertexData2));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void ChunkRenderer::startFrame(const Camera& camera, float worldLightLevel) const {
  const Shader* shader = ShaderManager::getShader("chunk");
  shader->use();
  shader->setFloat("u_WorldLightLevel", worldLightLevel);
  shader->setBool("u_UseAmbientOcclusion", Config::getUseAmbientOcclusion());
  shader->setInt("u_Texture", 0);
  shader->setInt("u_WaterTexIndex", m_waterTexIndex);
  shader->setFloat("u_Time", static_cast<float>(glfwGetTime()));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, textureAtlasID);
  Config::useWireFrame ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ChunkRenderer::updateGPUResources(ChunkMesh& mesh) {
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
  glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(ChunkVertex), (void*) offsetof(ChunkVertex, vertexData1));
  glEnableVertexAttribArray(0);
  glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(ChunkVertex), (void*) offsetof(ChunkVertex, vertexData2));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

}

