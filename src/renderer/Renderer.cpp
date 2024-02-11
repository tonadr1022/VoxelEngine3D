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
  m_chunkRenderer.render(world);
  // render block break and outline if a block is being aimed at
  if (world.getLastRayCastBlockPos() != NULL_VECTOR) {
    renderBlockOutline(world.player.camera, world.getLastRayCastBlockPos());
    renderBlockBreak(world.player.camera, world.getLastRayCastBlockPos(), world.player.blockBreakStage);
  }
  renderCrossHair();

  glDisable(GL_BLEND);
}
void Renderer::init() {
  m_chunkRenderer.init();
}

