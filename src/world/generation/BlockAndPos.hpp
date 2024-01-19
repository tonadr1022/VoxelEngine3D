//
// Created by Tony Adriansen on 1/19/24.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_GENERATION_BLOCKANDPOS_HPP_
#define VOXEL_ENGINE_SRC_WORLD_GENERATION_BLOCKANDPOS_HPP_

#include "../../EngineConfig.hpp"
#include "../block/Block.hpp"

struct BlockAndPos {
  glm::ivec3 pos;
  Block block;

  BlockAndPos(glm::ivec3 ppos, Block pblock) : pos(ppos), block(pblock) {}
};
#endif //VOXEL_ENGINE_SRC_WORLD_GENERATION_BLOCKANDPOS_HPP_
