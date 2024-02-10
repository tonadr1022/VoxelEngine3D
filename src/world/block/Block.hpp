//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_BLOCK_HPP
#define VOXEL_ENGINE_BLOCK_HPP

#include "../../EngineConfig.hpp"

enum class Block : uint8_t {
  AIR = 0,
  DIRT, // 1
  PLAINS_GRASS_BLOCK, // 2
  STONE, // 3
  GRAVEL, // 4
  BEDROCK,   // 5
  SAND, // 6
  OAK_WOOD, // 7
  OAK_LEAVES, // 8
  COBBLESTONE, // 9
  SNOWY_GRASS_BLOCK, // 10
  ICE_BLOCK, // 11
  WATER, // 12
  CRAFTING_TABLE, // 13
  GLOWSTONE, // 14
  JUNGLE_WOOD, // 15
  JUNGLE_LEAVES, // 16
  BIRCH_WOOD, // 17
  BIRCH_LEAVES, // 18
  SPRUCE_WOOD, // 19
  SPRUCE_LEAVES, // 20
  COAL_ORE, // 21
  IRON_ORE, // 22
  REDSTONE_ORE, // 23
  GOLD_ORE, // 24
  DIAMOND_ORE, // 25
  EMERALD_ORE, // 26
  LAPIS_ORE, // 27
  CLAY, // 28
  SUGAR_CANE, // 29
  CACTUS, // 30
  OAK_PLANKS, // 31
  BIRCH_PLANKS, // 32
  JUNGLE_PLANKS, // 33
  SPRUCE_PLANKS, // 34
  GLOWSTONE_RED, // 35
  GLOWSTONE_GREEN, // 36
  GLOWSTONE_BLUE, // 37
  SNOWY_TAIGA_GRASS_BLOCK, // 38
  DIRT_PATH, // 39
  JUNGLE_GRASS_BLOCK,
  DESERT_GRASS_BLOCK,
  DIAMOND_BLOCK,
  BLOCK_COUNT,
};

struct BlockTextureData {
  std::array<std::string, 6> sideFileNames{};
  std::array<int, 6> sideTexArrayIndices{};
};




//typedef uint8_t Block;

namespace BlockMethods {

}

#endif //VOXEL_ENGINE_BLOCK_HPP
