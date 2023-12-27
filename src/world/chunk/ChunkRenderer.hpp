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
  explicit ChunkRenderer(Camera &camera);

  ~ChunkRenderer();

  static void createGPUResources(Chunk &chunk);
  void render(Chunk &chunk);
  void start();
  void updateShaderUniforms();

 private:
  unsigned int textureAtlasID;
  Camera &camera;
  std::shared_ptr<Shader> shader;

};

#endif //VOXEL_ENGINE_CHUNKRENDERER_HPP
