//
// Created by Tony Adriansen on 12/28/2023.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHBUILDER_HPP_
#define VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHBUILDER_HPP_

#include "../../AppConstants.hpp"
#include "../../EngineConfig.hpp"
#include "../../world/block/Block.hpp"
#include "ChunkMesh.hpp"

class Chunk;

using OcclusionLevels = std::array<uint8_t, 4>;

class ChunkMeshBuilder {
 public:
  ChunkMeshBuilder(const Chunk &chunk0, const Chunk &chunk1, const Chunk &chunk2, const Chunk &chunk3,
                   const Chunk &chunk4, const Chunk &chunk5, const Chunk &chunk6, const Chunk &chunk7,
                   const Chunk &chunk8);

  void constructMesh(std::vector<ChunkVertex> &opaqueVertices, std::vector<unsigned int> &opaqueIndices,
                     std::vector<ChunkVertex> &transparentVertices, std::vector<unsigned int> &transparentIndices);

  OcclusionLevels getOcclusionLevels(const glm::ivec3 &blockPosInChunk,
                                     BlockFace face);

 private:
  Block getBlock(int x, int y, int z);

  const Chunk &m_chunk0;
  const Chunk &m_chunk1;
  const Chunk &m_chunk2;
  const Chunk &m_chunk3;
  const Chunk &m_chunk4;
  const Chunk &m_chunk5;
  const Chunk &m_chunk6;
  const Chunk &m_chunk7;
  const Chunk &m_chunk8;
};

#endif //VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHBUILDER_HPP_
