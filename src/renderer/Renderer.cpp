//
// Created by Tony Adriansen on 11/16/23.
//

#include "Renderer.hpp"
#include "../shaders/ShaderManager.hpp"
#include "../resources/ResourceManager.hpp"
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
  m_chunkRenderer.start(world.player.camera);
  for (auto &pos : world.getOpaqueRenderSet()) {
    Chunk *chunk = world.getChunkRawPtr(pos);
    if (!chunk) continue;
    m_chunkRenderer.render(chunk->m_meshes[0], chunk->m_worldPos);
  }

  // render block break and outline if a block is being aimed at
  if (static_cast<const glm::vec3>(world.getLastRayCastBlockPos()) != NULL_VECTOR) {
    renderBlockOutline(world.player.camera, world.getLastRayCastBlockPos());
    renderBlockBreak(world.player.camera, world.getLastRayCastBlockPos(), world.player.blockBreakStage);
  }
  renderCrossHair();
}


