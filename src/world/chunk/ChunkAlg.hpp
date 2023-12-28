//
// Created by Tony Adriansen on 12/23/23.
//

#ifndef VOXEL_ENGINE_CHUNKALG_HPP
#define VOXEL_ENGINE_CHUNKALG_HPP

#include "../../AppConstants.hpp"
#include "../../EngineConfig.hpp"
#include "../../world/block/Block.hpp"
#include "ChunkMesh.hpp"

namespace ChunkAlg {

using OcclusionLevels = std::array<uint8_t, 4>;

extern void constructMesh(const Chunk &chunk0,
                          const Chunk &chunk1,
                          const Chunk &chunk2,
                          const Chunk &chunk3,
                          const Chunk &chunk4,
                          const Chunk &chunk5,
                          const Chunk &chunk6,
                          const Chunk &chunk7,
                          const Chunk &chunk8,
                          std::vector<ChunkVertex> &vertices,
                          std::vector<unsigned int> &indices);

extern OcclusionLevels getOcclusionLevels(glm::ivec3 &blockPosInChunk,
                                          BlockFace face,
                                          const Chunk &chunk0,
                                          const Chunk &chunk1,
                                          const Chunk &chunk2,
                                          const Chunk &chunk3,
                                          const Chunk &chunk4,
                                          const Chunk &chunk5,
                                          const Chunk &chunk6,
                                          const Chunk &chunk7,
                                          const Chunk &chunk8);
};

#endif //VOXEL_ENGINE_CHUNKALG_HPP
