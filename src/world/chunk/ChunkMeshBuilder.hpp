//
// Created by Tony Adriansen on 12/28/2023.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHBUILDER_HPP_
#define VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHBUILDER_HPP_

#include "../../AppConstants.hpp"
#include "../../EngineConfig.hpp"
#include "../../world/block/Block.hpp"
#include "ChunkMesh.hpp"
#include "../../utils/Utils.hpp"
#include "ChunkVertex.hpp"

class ChunkMeshBuilder {
 public:
 explicit ChunkMeshBuilder(Block (&blocks)[CHUNK_MESH_INFO_SIZE], uint16_t (&lightLevels)[CHUNK_MESH_INFO_SIZE],
                           uint8_t (&sunlightLevels)[CHUNK_MESH_INFO_SIZE], const glm::ivec3 &chunkWorldPos);

   void constructMesh(std::vector<ChunkVertex> &opaqueVertices, std::vector<unsigned int> &opaqueIndices,
                     std::vector<ChunkVertex> &transparentVertices, std::vector<unsigned int> &transparentIndices);

   void setOcclusionLevels(const glm::ivec3 &blockPosInChunk, BlockFace face, int (&levels)[4]);
//   static void setOcclusionLevels(bool (&solidNeighborBlocks)[27], int faceIndex, int (&levels)[4]);

 private:
  Block (&m_blocks)[CHUNK_MESH_INFO_SIZE];
  uint16_t (&m_lightLevels)[CHUNK_MESH_INFO_SIZE];
  uint8_t (&m_sunlightLevels)[CHUNK_MESH_INFO_SIZE];
  glm::ivec3 m_chunkWorldPos;

//  Chunk *(m_chunks)[27] = {nullptr};
};

#endif //VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHBUILDER_HPP_
