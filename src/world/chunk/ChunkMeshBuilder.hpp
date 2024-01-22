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
#include "../block/BlockDB.hpp"
struct FaceInfo {
  uint8_t aoLevels[4];
  uint16_t torchLightLevel;
  uint8_t sunlightLevel;
  bool flip;

  void setValues(uint8_t faceNum, const Block (&blockNeighbors)[27], uint16_t torchlightLevel, uint8_t sunlightLevel);

  bool operator==(const FaceInfo &other) const {
    return torchLightLevel == other.torchLightLevel && sunlightLevel == other.sunlightLevel
        && aoLevels[0] == other.aoLevels[0] && aoLevels[1] == other.aoLevels[1] && aoLevels[2] == other.aoLevels[2]
        && aoLevels[3] == other.aoLevels[3];
  }

  bool operator!=(const FaceInfo &other) const {
    return torchLightLevel != other.torchLightLevel || sunlightLevel != other.sunlightLevel
        || aoLevels[0] != other.aoLevels[0] || aoLevels[1] != other.aoLevels[1] || aoLevels[2] != other.aoLevels[2]
        || aoLevels[3] != other.aoLevels[3];
  }
};

class ChunkMeshBuilder {
 public:
  explicit ChunkMeshBuilder(Block (&blocks)[CHUNK_MESH_INFO_SIZE], uint16_t (&lightLevels)[CHUNK_MESH_INFO_SIZE],
                            uint8_t (&sunlightLevels)[CHUNK_MESH_INFO_SIZE], const glm::ivec3 &chunkWorldPos);

  void constructMesh(std::vector<ChunkVertex> &opaqueVertices, std::vector<unsigned int> &opaqueIndices,
                     std::vector<ChunkVertex> &transparentVertices, std::vector<unsigned int> &transparentIndices);
  void constructMeshGreedy(std::vector<ChunkVertex> &opaqueVertices,
                           std::vector<unsigned int> &opaqueIndices,
                           std::vector<ChunkVertex> &transparentVertices,
                           std::vector<unsigned int> &transparentIndices);

  void setOcclusionLevels(const glm::ivec3 &blockPosInChunk, int faceIndex, int (&levels)[4]);
//   static void setOcclusionLevels(bool (&solidNeighborBlocks)[27], int faceIndex, int (&levels)[4]);

// TODO: improve for other edge cases: glass, leaves, etc.
  static inline bool shouldShowFace(Block block, Block neighborBlock) {
    if (block == Block::AIR) return false; // if block to show face is air, dont show face
    if (neighborBlock == Block::AIR) return true; // to avoid lookup??? profile
    const BlockData &blockData = BlockDB::getBlockData(block);
    const BlockData &neighborData = BlockDB::getBlockData(neighborBlock);
    if (!blockData.isTransparent && neighborData.isTransparent) return true;
    if (!neighborData.isTransparent) return false;
    if (!blockData.cull && !neighborData.cull) return true;
    if (block == neighborBlock) return false;
//    if (neighborData.isTransparent && block != Block::WATER) return true;
    return true;
  }

 private:
  Block (&m_blocks)[CHUNK_MESH_INFO_SIZE];
  uint16_t (&m_lightLevels)[CHUNK_MESH_INFO_SIZE];
  uint8_t (&m_sunlightLevels)[CHUNK_MESH_INFO_SIZE];
  glm::ivec3 m_chunkWorldPos;

//  Chunk *(m_chunks)[27] = {nullptr};
};

#endif //VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKMESHBUILDER_HPP_
