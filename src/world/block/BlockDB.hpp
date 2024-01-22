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
  std::vector<std::string> texFilenames;
  std::array<int, 6> texIndex;
  bool isTransparent;
  bool isCollidable;
  bool isLightSource;
  bool cull;
  uint32_t packedLightLevel;
  glm::ivec3 lightLevel;
  uint8_t lightAttenuation;
};

class BlockDB {
 public:
  static Block blockIdFromName(const std::string &name);
  static void loadData();

  static inline BlockData &getBlockData(Block id) {
//    return data.find(id)->second;
    return data[static_cast<int>(id)];
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

  static inline uint32_t getPackedLightLevel(Block id) {
    return getBlockData(id).packedLightLevel;
  }

  static inline uint8_t lightAttenuation(Block id) {
    return getBlockData(id).lightAttenuation;
  }

  static inline bool isTransparent(Block id) {
    return getBlockData(id).isTransparent;
  }

  static inline bool shouldCull(Block id) {
    return getBlockData(id).cull;
  }

 private:
  static std::vector<BlockData> data;
  static std::unordered_map<std::string, Block> blockNameToIdMap;

};

#endif //VOXEL_ENGINE_BLOCKDB_HPP
