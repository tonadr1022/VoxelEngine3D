//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CHUNKRENDERER_HPP
#define VOXEL_ENGINE_CHUNKRENDERER_HPP

#include "../shaders/Shader.hpp"
#include "../camera/Camera.hpp"
#include "../EngineConfig.hpp"
#include "../world/chunk/ChunkMesh.hpp"

class World;

class ChunkRenderer {
 public:
  ChunkRenderer();

  ~ChunkRenderer();

  void init();
  void render(const World& world) const;
 private:
  static void createGPUResources(ChunkMesh& mesh);
  static void updateGPUResources(ChunkMesh& mesh);
  void startFrame(const Camera& camera, float worldLightLevel) const;
  void renderChunk(ChunkMesh& mesh, const Camera& camera, const glm::ivec3& worldPos, float firstBufferTime) const;
  unsigned int textureAtlasID{};
  int m_waterTexIndex{};
};

#endif //VOXEL_ENGINE_CHUNKRENDERER_HPP
