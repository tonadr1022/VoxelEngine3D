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
#include "../block/BlockDB.hpp"

using OcclusionLevels = std::array<uint8_t, 4>;

struct FaceInfo {
  uint8_t aoLevels[4];
  bool flip;

  void setValues(uint8_t faceNum, const Block (&blockNeighbors)[27]);

  bool operator==(const FaceInfo &other) const {
    return aoLevels[0] == other.aoLevels[0] && aoLevels[1] == other.aoLevels[1] && aoLevels[2] == other.aoLevels[2] &&
        aoLevels[3] == other.aoLevels[3];
  }

  bool operator!=(const FaceInfo &other) const {
    return aoLevels[0] != other.aoLevels[0] || aoLevels[1] != other.aoLevels[1] || aoLevels[2] != other.aoLevels[2] ||
        aoLevels[3] != other.aoLevels[3];
  }
};

class ChunkMeshBuilder {
 public:
  explicit ChunkMeshBuilder(Block (&blocks)[CHUNK_MESH_INFO_SIZE], const glm::ivec3 &chunkWorldPos);

  void constructMesh(std::vector<ChunkVertex> &opaqueVertices, std::vector<unsigned int> &opaqueIndices,
                     std::vector<ChunkVertex> &transparentVertices, std::vector<unsigned int> &transparentIndices);

  static void constructMeshGreedy(std::vector<ChunkVertex> &opaqueVertices, std::vector<unsigned int> &opaqueIndices,
                           std::vector<ChunkVertex> &transparentVertices, std::vector<unsigned int> &transparentIndices, Block (&blocks)[CHUNK_MESH_INFO_SIZE]);

  OcclusionLevels getOcclusionLevels(const glm::ivec3 &blockPosInChunk, BlockFace face);

 private:
  Block (&m_blocks)[CHUNK_MESH_INFO_SIZE];
  glm::ivec3 m_chunkPos;

  // TODO: improve for other edge cases: glass, leaves, etc.
  static inline bool shouldShowFace(uint8_t block, uint8_t neighborBlock) {
    if (!block) return false; // if block to show face is air, dont show face
    if (!neighborBlock) return true; // to avoid lookup??? profile
    bool blockIsTrans = BlockDB::isTransparent(block);
    bool neighborIsTrans = BlockDB::isTransparent(neighborBlock);
    if (blockIsTrans || !neighborIsTrans) return false;
    return true;
  }

//  Chunk *(m_chunks)[27] = {nullptr};
};

#endif //VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHBUILDER_HPP_
