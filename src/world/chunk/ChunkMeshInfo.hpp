//
// Created by Tony Adriansen on 1/1/2024.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHINFO_HPP_
#define VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHINFO_HPP_

#include "../../EngineConfig.hpp"
#include "ChunkInfo.hpp"
#include "../block/Block.hpp"
#include "../../AppConstants.hpp"
#include "ChunkVertex.hpp"

class Chunk;
class ChunkMeshInfo : public ChunkInfo {
 public:
  explicit ChunkMeshInfo(Chunk *chunk);
  void generateMeshData(bool greedy);
  void generateLODMeshData(int lodScale);
  void applyMeshDataToMesh();
  static void populateMeshInfoForMeshing(Block (&blockResult)[CHUNK_MESH_INFO_SIZE],
                                         uint16_t (&torchResult)[CHUNK_MESH_INFO_SIZE],
                                         uint8_t (&sunlightResult)[CHUNK_MESH_INFO_SIZE],
                                         Chunk *(&chunks)[27]);


 private:
  Chunk *m_chunk;
  std::vector<ChunkVertex> m_opaqueVertices;
  std::vector<ChunkVertex> m_transparentVertices;
  std::vector<unsigned int> m_opaqueIndices;
  std::vector<unsigned int> m_transparentIndices;
};

#endif //VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHINFO_HPP_
