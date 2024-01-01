//
// Created by Tony Adriansen on 11/16/23.
//

#include "Renderer.hpp"
#include "../world/World.hpp"

Renderer::Renderer() = default;

void Renderer::renderBlockOutline(const Camera &camera, glm::ivec3 blockPosition) const {
  m_blockOutlineRenderer.render(blockPosition, camera);
}

void Renderer::renderBlockBreak(const Camera &camera,
                                glm::ivec3 blockPosition,
                                int breakStage) const {
  m_blockBreakRenderer.render(blockPosition, camera, breakStage);
}

void Renderer::renderCrossHair() const {
  crossHair.render();
}

void Renderer::renderWorld(const World &world) {
  m_viewFrustum.updatePlanes(world.player.camera.getProjectionMatrix(), world.player.camera.getViewMatrix());
  m_chunkRenderer.start(world.player.camera);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  for (auto &pos : world.getOpaqueRenderSet()) {
    Chunk *chunk = world.getChunkRawPtrOrNull(pos);
    if (!chunk) continue;
    if (!m_viewFrustum.isBoxInFrustum(chunk->m_boundingBox)) continue;
    if (chunk->m_firstBufferTime == 0) chunk->m_firstBufferTime = static_cast<float>(glfwGetTime());
    m_chunkRenderer.render(chunk->m_opaqueMesh, chunk->m_worldPos, chunk->m_firstBufferTime);
  }


  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  for (auto &pos : world.getTransparentRenderVector()) {
    Chunk *chunk = world.getChunkRawPtrOrNull(pos);
    if (!chunk) continue;
    if (!m_viewFrustum.isBoxInFrustum(chunk->m_boundingBox)) continue;
    m_chunkRenderer.render(chunk->m_transparentMesh, chunk->m_worldPos, chunk->m_firstBufferTime);

  }
  // render block break and outline if a block is being aimed at
  if (static_cast<const glm::vec3>(world.getLastRayCastBlockPos()) != NULL_VECTOR) {
    renderBlockOutline(world.player.camera, world.getLastRayCastBlockPos());
    renderBlockBreak(world.player.camera, world.getLastRayCastBlockPos(), world.player.blockBreakStage);
  }
  renderCrossHair();

  glDisable(GL_BLEND);
}


