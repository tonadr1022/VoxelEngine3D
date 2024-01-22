//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CHUNKRENDERER_HPP
#define VOXEL_ENGINE_CHUNKRENDERER_HPP

#include "../../shaders/Shader.hpp"
#include "../../camera/Camera.hpp"
#include "../../EngineConfig.hpp"
#include "../chunk/ChunkMesh.hpp"

class ChunkRenderer {
 public:
  ChunkRenderer();

  ~ChunkRenderer();

  static void createGPUResources(ChunkMesh &mesh);
  static void updateGPUResources(ChunkMesh &mesh);
  void render(ChunkMesh &mesh, const glm::ivec3 &worldPos, float firstBufferTime);
  void start(const Camera &camera, float worldLightLevel);
  void updateShaderUniforms(const Camera &camera, float worldLightLevel);

 private:
  unsigned int textureAtlasID;
};

#endif //VOXEL_ENGINE_CHUNKRENDERER_HPP
