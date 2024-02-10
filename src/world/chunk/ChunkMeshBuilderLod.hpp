//
// Created by Tony Adriansen on 2/10/24.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHBUILDERLOD_HPP_
#define VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHBUILDERLOD_HPP_
#include "../../EngineConfig.hpp"
#include "../../AppConstants.hpp"
#include "../../world/block/Block.hpp"
#include "ChunkVertex.hpp"

class Chunk;

namespace ChunkMeshBuilderLod {

  extern void constructMeshGreedyLOD(std::vector<ChunkVertex>& opaqueVertices,
                              std::vector<unsigned int>& opaqueIndices,
                              std::vector<ChunkVertex>& transparentVertices,
                              std::vector<unsigned int>& transparentIndices, Chunk* chunk, int lodScale);
  extern uint32_t createLODVertexData(uint8_t x, uint8_t y, uint8_t z, uint8_t u, uint8_t v);
}

#endif //VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHBUILDERLOD_HPP_
