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
  explicit ChunkMeshBuilder(Chunk *(&chunks)[9]);

  void constructMesh(std::vector<uint32_t> &opaqueVertices, std::vector<unsigned int> &opaqueIndices,
                     std::vector<uint32_t> &transparentVertices, std::vector<unsigned int> &transparentIndices);

  OcclusionLevels getOcclusionLevels(const glm::ivec3 &blockPosInChunk, BlockFace face);

 private:
  Block getBlock(int x, int y, int z);

  Chunk *(m_chunks)[9] = {nullptr};
};

#endif //VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHBUILDER_HPP_
