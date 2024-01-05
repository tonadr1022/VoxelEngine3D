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
  bool isLightSource;
  uint32_t packedLightLevel;
  glm::ivec3 lightLevel;
  bool lightCanPass;
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

  static inline bool isLightSource(Block id) {
    return getBlockData(id).isLightSource;
  }

  static inline glm::ivec3 getLightLevel(Block id) {
    return getBlockData(id).lightLevel;
  }

  static inline uint32_t getpackedLightLevel(Block id) {
    return getBlockData(id).packedLightLevel;
  }

  static inline bool canLightPass(Block id) {
    return getBlockData(id).lightCanPass;
  }

 private:
  static std::vector<BlockData> data;
};

#endif //VOXEL_ENGINE_BLOCKDB_HPP
