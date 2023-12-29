//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_RENDERER_HPP
#define VOXEL_ENGINE_RENDERER_HPP

#include "../shaders/Shader.hpp"
#include "../gui/CrossHair.hpp"
#include "BlockOutlineRenderer.hpp"
#include "BlockBreakRenderer.hpp"
#include "../world/chunk/ChunkRenderer.hpp"

class World;

class Renderer {
 public:
  explicit Renderer();

  void renderCrossHair() const;
  void renderBlockOutline(const Camera &camera, glm::ivec3 blockPosition) const;
  void renderBlockBreak(const Camera &camera,
                        glm::ivec3 blockPosition,
                        int breakStage) const;
  void renderWorld(const World &world);

 private:
  BlockOutlineRenderer m_blockOutlineRenderer;
  BlockBreakRenderer m_blockBreakRenderer;
  ChunkRenderer m_chunkRenderer;
  CrossHair crossHair;

};

#endif //VOXEL_ENGINE_RENDERER_HPP
