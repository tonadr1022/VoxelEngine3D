//
// Created by Tony Adriansen on 11/22/23.
//

#include "BlockDB.hpp"
#include "json/json.hpp"
#include "../../utils/Utils.hpp"
#include "../../resources/ResourceManager.hpp"
#include "../../utils/JsonUtils.hpp"

//std::unordered_map<Block, BlockData> BlockDB::data;
std::vector<BlockData> BlockDB::data;

std::unordered_map<std::string, Block> BlockDB::blockNameToIdMap = {
    {"Water", Block::WATER},
    {"Stone", Block::STONE},
    {"Snowy Grass Block", Block::SNOWY_GRASS_BLOCK},
    {"Sand", Block::SAND},
    {"Oak Wood", Block::OAK_WOOD},
    {"Oak Leaves", Block::OAK_LEAVES},
    {"Jungle Wood", Block::JUNGLE_WOOD},
    {"Gravel", Block::GRAVEL},
    {"Glowstone", Block::GLOWSTONE},
    {"Red Glowstone", Block::GLOWSTONE_RED},
    {"Green Glowstone", Block::GLOWSTONE_GREEN},
    {"Blue Glowstone", Block::GLOWSTONE_BLUE},
    {"Dirt", Block::DIRT},
    {"Diamond Block", Block::DIAMOND_BLOCK},
    {"Crafting Table", Block::CRAFTING_TABLE},
    {"Cobblestone", Block::COBBLESTONE},
    {"Bedrock", Block::BEDROCK},
    {"Ice Block", Block::ICE_BLOCK},
    {"Air", Block::AIR},
    {"Jungle Leaves", Block::JUNGLE_LEAVES},
    {"Birch Wood", Block::BIRCH_WOOD},
    {"Birch Leaves", Block::BIRCH_LEAVES},
    {"Spruce Leaves", Block::SPRUCE_LEAVES},
    {"Coal Ore", Block::COAL_ORE},
    {"Iron Ore", Block::IRON_ORE},
    {"Redstone Ore", Block::REDSTONE_ORE},
    {"Gold Ore", Block::GOLD_ORE},
    {"Diamond Ore", Block::DIAMOND_ORE},
    {"Emerald Ore", Block::EMERALD_ORE},
    {"Lapis Lazuli Ore", Block::LAPIS_ORE},
    {"Clay", Block::CLAY},
    {"Sugar Cane", Block::SUGAR_CANE},
    {"Cactus", Block::CACTUS},
    {"Oak Planks", Block::OAK_PLANKS},
    {"Birch Planks", Block::BIRCH_PLANKS},
    {"Jungle Planks", Block::JUNGLE_PLANKS},
    {"Spruce Planks", Block::SPRUCE_PLANKS},
    {"Plains Grass Block", Block::PLAINS_GRASS_BLOCK},
    {"Snowy Taiga Grass Block", Block::SNOWY_TAIGA_GRASS_BLOCK},
    {"Desert Grass Block", Block::DESERT_GRASS_BLOCK},
    {"Jungle Grass Block", Block::JUNGLE_GRASS_BLOCK},
    {"Spruce Wood", Block::SPRUCE_WOOD},
    {"Dirt Path", Block::DIRT_PATH}
};

void BlockDB::loadData(const std::string &filePath) {
  nlohmann::json blocks = JsonUtils::openJson(filePath + "block_data.json");
std::vector<BlockData> prelimData;
  for (const auto &[blockName, blockData] : blocks.items()) {
    BlockData blockDataEntry{};
    blockDataEntry.name = blockName;
    blockDataEntry.id = blockIdFromName(blockName);
    /* length    layout
 * 1          all
 * 2          top, rest
 * 3          top, side, bottom
 * 4          top, front, side, bottom
 * 6          front, back, top, bottom, right, left
*/
    std::vector<std::string> v = blockData["texFilenames"].get<std::vector<std::string>>();
    blockDataEntry.texFilenames = v;
    int filenamesSize = blockDataEntry.texFilenames.size();
    if (filenamesSize == 1) {
      int index = ResourceManager::filenameToTexIndex[blockDataEntry.texFilenames[0]];
      std::fill(blockDataEntry.texIndex.begin(),
                blockDataEntry.texIndex.end(),
                index);
    } else if (filenamesSize == 2) {
      blockDataEntry.texIndex[2] = ResourceManager::filenameToTexIndex[blockDataEntry.texFilenames[0]];
      blockDataEntry.texIndex[0] = ResourceManager::filenameToTexIndex[blockDataEntry.texFilenames[1]];
      blockDataEntry.texIndex[1] = ResourceManager::filenameToTexIndex[blockDataEntry.texFilenames[1]];
      blockDataEntry.texIndex[3] = ResourceManager::filenameToTexIndex[blockDataEntry.texFilenames[1]];
      blockDataEntry.texIndex[4] = ResourceManager::filenameToTexIndex[blockDataEntry.texFilenames[1]];
      blockDataEntry.texIndex[5] = ResourceManager::filenameToTexIndex[blockDataEntry.texFilenames[1]];
    } else if (filenamesSize == 3) {
      blockDataEntry.texIndex[0] = ResourceManager::filenameToTexIndex[blockDataEntry.texFilenames[1]];
      blockDataEntry.texIndex[1] = ResourceManager::filenameToTexIndex[blockDataEntry.texFilenames[1]];
      blockDataEntry.texIndex[2] = ResourceManager::filenameToTexIndex[blockDataEntry.texFilenames[1]];
      blockDataEntry.texIndex[3] = ResourceManager::filenameToTexIndex[blockDataEntry.texFilenames[1]];
      blockDataEntry.texIndex[4] = ResourceManager::filenameToTexIndex[blockDataEntry.texFilenames[0]];
      blockDataEntry.texIndex[5] = ResourceManager::filenameToTexIndex[blockDataEntry.texFilenames[2]];
    }
    blockDataEntry.isTransparent = blockData["isTransparent"].get<bool>();
    blockDataEntry.isCollidable = blockData["isCollidable"].get<bool>();
    blockDataEntry.isLightSource = blockData["isLightSource"].get<bool>();

    auto unpacked = blockData["lightLevels"].get<std::array<int, 3>>();
    blockDataEntry.lightLevel =
        {static_cast<int8_t>(unpacked[0]), static_cast<int8_t>(unpacked[1]), static_cast<int8_t>(unpacked[2])};
    blockDataEntry.packedLightLevel = Utils::packLightLevel(blockDataEntry.lightLevel);
    blockDataEntry.lightCanPass = blockData["lightCanPass"].get<bool>();

    prelimData.push_back(blockDataEntry);
  }

  std::sort(prelimData.begin(), prelimData.end(), [](const BlockData &a, const BlockData &b) { return a.id < b.id; });
  for (const auto &blockDataEntry : prelimData) {
    data.push_back(blockDataEntry);
  }
}
Block BlockDB::blockIdFromName(const std::string &name) {
  return blockNameToIdMap[name];
}




