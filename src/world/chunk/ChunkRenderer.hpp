//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CHUNKRENDERER_HPP
#define VOXEL_ENGINE_CHUNKRENDERER_HPP

#include "../../shaders/Shader.hpp"
#include "Chunk.hpp"
#include "../../camera/Camera.hpp"
#include "../../EngineConfig.hpp"

class ChunkRenderer {
 public:
  ChunkRenderer();

  ~ChunkRenderer();

  static void createGPUResources(ChunkMesh &mesh);
  static void updateGPUResources(ChunkMesh &mesh);
  void render(ChunkMesh &mesh, const glm::ivec2 &worldPos, float firstBufferTime);
  void start(const Camera &camera);
  void updateShaderUniforms(const Camera &camera);

 private:
  unsigned int textureAtlasID;
  std::shared_ptr<Shader> shader;

};

#endif //VOXEL_ENGINE_CHUNKRENDERER_HPP
