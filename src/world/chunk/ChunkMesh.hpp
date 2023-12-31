//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CHUNKMESH_HPP
#define VOXEL_ENGINE_CHUNKMESH_HPP

#include "../block/Block.hpp"
#include "../../EngineConfig.hpp"
#include "ChunkVertex.hpp"

class Chunk;

enum BlockFace : uint8_t {
  FRONT = 0,
  BACK,
  RIGHT,
  LEFT,
  TOP,
  BOTTOM
};

class ChunkMesh {
 public:
  ChunkMesh();
  ~ChunkMesh() = default;

  void clearData();
  void clearBuffers();

  std::vector<ChunkVertex> vertices;
  std::vector<unsigned int> indices;
  bool isBuffered = false;
  bool needsUpdate = false;
  unsigned int VAO, VBO, EBO;

 private:

};

#endif //VOXEL_ENGINE_CHUNKMESH_HPP
