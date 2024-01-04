//
// Created by Tony Adriansen on 11/22/23.
//

#ifndef VOXEL_ENGINE_BLOCKDB_HPP
#define VOXEL_ENGINE_BLOCKDB_HPP

#include "Block.hpp"
#include "../../EngineConfig.hpp"

struct BlockData {
  Block id;
  std::string name;
  std::array<int, 6> texIndex;
  bool isTransparent;
  bool isCollidable;
};

class BlockDB {
 public:
  static void loadData(const std::string &filePath);

  static inline BlockData &getBlockData(Block id) {
    return data[static_cast<unsigned long>(id)];
  }

  static inline int getTexIndex(Block id, int face) {
    return getBlockData(id).texIndex[face];
  }

 private:
  static std::vector<BlockData> data;
};

#endif //VOXEL_ENGINE_BLOCKDB_HPP
