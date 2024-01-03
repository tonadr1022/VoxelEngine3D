//
// Created by Tony Adriansen on 11/22/23.
//

#ifndef VOXEL_ENGINE_BLOCKDB_HPP
#define VOXEL_ENGINE_BLOCKDB_HPP

#include "Block.hpp"
#include "../../EngineConfig.hpp"
#include "../chunk/ChunkMesh.hpp"
#include "../../AppConstants.hpp"

struct BlockData {
  Block id;
  std::string name;
  // front back right left top bottom
  std::array<int, 6> texCoords;
  bool isTransparent;
  bool isCollidable;
};

class BlockDB {
 public:
  static void loadData(const std::string &filePath);
  static BlockData &getBlockData(Block id);

//  static inline bool isTransparent(uint8_t id) {
//    return !id ||  data[static_cast<unsigned long>(id)].isTransparent;
//  }

  static inline bool isTransparent(uint8_t id) {
    return !id ||  static_cast<Block>(id) == Block::WATER || static_cast<Block>(id) == Block::OAK_LEAVES || static_cast<Block>(id)
        == Block::JUNGLE_LEAVES ||
        static_cast<Block>(id) == Block::BIRCH_LEAVES || static_cast<Block>(id) == Block::SPRUCE_LEAVES;
  }

  static inline int getTextureIndex(Block id, BlockFace faceNum) {
    return data[static_cast<unsigned long>(id)].texCoords[faceNum];
  }

 private:
  static std::vector<BlockData> data;
};

#endif //VOXEL_ENGINE_BLOCKDB_HPP
