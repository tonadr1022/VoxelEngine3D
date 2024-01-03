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

  glm::ivec2 topTexCoords;
  glm::ivec2 frontTexCoords;
  glm::ivec2 backTexCoords;
  glm::ivec2 leftTexCoords;
  glm::ivec2 rightTexCoords;
  glm::ivec2 bottomTexCoords;
  bool isTransparent;
  bool isCollidable;
};

class BlockDB {
 public:
  static void loadData(const std::string &filePath);
  static BlockData &getBlockData(Block id);

  static inline bool isTransparent(Block id) {
    return data[static_cast<unsigned long>(id)].isTransparent;
  }

  static inline int getTextureIndex(Block id, BlockFace faceNum) {
    int textureX, textureY;
    BlockData &blockData = data[static_cast<unsigned long>(id)];
    switch (faceNum) {
      case BlockFace::FRONT:textureX = blockData.frontTexCoords.x;
        textureY = blockData.frontTexCoords.y;
        break;
      case BlockFace::BACK:textureX = blockData.backTexCoords.x;
        textureY = blockData.backTexCoords.y;
        break;
      case BlockFace::LEFT:textureX = blockData.leftTexCoords.x;
        textureY = blockData.leftTexCoords.y;
        break;
      case BlockFace::RIGHT:textureX = blockData.rightTexCoords.x;
        textureY = blockData.rightTexCoords.y;
        break;
      case BlockFace::TOP:textureX = blockData.topTexCoords.x;
        textureY = blockData.topTexCoords.y;
        break;
      case BlockFace::BOTTOM:textureX = blockData.bottomTexCoords.x;
        textureY = blockData.bottomTexCoords.y;
        break;
      default:break;
    }
//    return textureX * TEXTURE_ATLAS_WIDTH + textureY;
return 207;
//return 127;
  }

 private:
  static std::vector<BlockData> data;
};

#endif //VOXEL_ENGINE_BLOCKDB_HPP
