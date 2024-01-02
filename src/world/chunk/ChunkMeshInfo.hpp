//
// Created by Tony Adriansen on 1/1/2024.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHINFO_HPP_
#define VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHINFO_HPP_

#include "../../EngineConfig.hpp"
#include "ChunkInfo.hpp"
#include "../block/Block.hpp"
#include "../../AppConstants.hpp"

class Chunk;
class ChunkMeshInfo : public ChunkInfo {
 public:
  explicit ChunkMeshInfo(Chunk *chunks[27]);
  void generateMeshData();
  void applyMeshDataToMesh(Chunk *chunk);
  static void populateMeshInfoForMeshing(Block (&result)[CHUNK_MESH_INFO_SIZE], Chunk *(&chunks)[27]);

 private:
  Chunk *m_chunks[27]{};
  std::vector<uint32_t> m_opaqueVertices;
  std::vector<uint32_t> m_transparentVertices;
  std::vector<unsigned int> m_opaqueIndices;
  std::vector<unsigned int> m_transparentIndices;
};

#endif //VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHINFO_HPP_
