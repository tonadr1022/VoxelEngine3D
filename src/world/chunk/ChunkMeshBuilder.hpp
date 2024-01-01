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

class Chunk;

using OcclusionLevels = std::array<uint8_t, 4>;

class ChunkMeshBuilder {
 public:
//  explicit ChunkMeshBuilder(Chunk *(&chunks)[27]);

  static void constructMesh(std::vector<uint32_t> &opaqueVertices, std::vector<unsigned int> &opaqueIndices,
                     std::vector<uint32_t> &transparentVertices, std::vector<unsigned int> &transparentIndices, Chunk *(&chunks)[27]);

  static OcclusionLevels getOcclusionLevels(const glm::ivec3 &blockPosInChunk, BlockFace face, Chunk *(&chunks)[27]);

 private:

  static inline Block getBlock(int x, int y, int z, Chunk *(&chunks)[27]);

//  Chunk *(m_chunks)[27] = {nullptr};
};

#endif //VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHBUILDER_HPP_
