//
// Created by Tony Adriansen on 12/23/23.
//

#ifndef VOXEL_ENGINE_CHUNKALG_HPP
#define VOXEL_ENGINE_CHUNKALG_HPP

#include "../../AppConstants.hpp"
#include "../../EngineConfig.hpp"
#include "../../world/block/Block.hpp"
#include "../../world/chunk/ChunkKey.hpp"
#include "ChunkMesh.hpp"


namespace ChunkAlg {

    using OcclusionLevels = std::array<uint8_t, 4>;

    extern void constructMesh(const Block (&blocks)[CHUNK_MESH_INFO_SIZE], ChunkKey chunkKey, std::vector<uint32_t> &vertices, std::vector<unsigned int> &indices);
    extern OcclusionLevels getOcclusionLevels(glm::ivec3 &blockPosInChunk, BlockFace face, const Block (&blocks)[CHUNK_MESH_INFO_SIZE]);
};


#endif //VOXEL_ENGINE_CHUNKALG_HPP
