//
// Created by Tony Adriansen on 11/22/23.
//

#include "BlockDB.hpp"
#include "json/json.hpp"
#include "../../utils/Utils.hpp"

//std::unordered_map<Block, BlockData> BlockDB::data;
std::vector<BlockData> BlockDB::data;

void BlockDB::loadData(const std::string &filePath) {
  std::filesystem::path jsonPath = filePath + "block_data.json";
  std::ifstream jsonFile(jsonPath);
  if (!jsonFile.is_open()) {
    throw std::runtime_error(
        "Failed to open file: " + filePath + "block_data.json");
  }
  nlohmann::json blocks;
  jsonFile >> blocks;
  jsonFile.close();

  std::vector<BlockData> prelimData;
  for (const auto &[blockName, blockData] : blocks.items()) {
    BlockData blockDataEntry{};
    blockDataEntry.name = blockName;
    blockDataEntry.id = static_cast<Block>(blockData["id"].get<int>());
    blockDataEntry.texIndex = blockData["texIndex"].get<std::array<int, 6>>();
    blockDataEntry.isTransparent = blockData["isTransparent"].get<bool>();
    blockDataEntry.isCollidable = blockData["isCollidable"].get<bool>();
    blockDataEntry.isLightSource = blockData["isLightSource"].get<bool>();

    auto unpacked = blockData["lightLevels"].get<std::array<int, 3>>();
    blockDataEntry.lightLevel = {static_cast<int8_t>(unpacked[0]), static_cast<int8_t>(unpacked[1]), static_cast<int8_t>(unpacked[2])};
    blockDataEntry.packedLightLevel = Utils::packLightLevel(blockDataEntry.lightLevel);
    blockDataEntry.lightCanPass = blockData["lightCanPass"].get<bool>();

    prelimData.push_back(blockDataEntry);
  }

  std::sort(prelimData.begin(), prelimData.end(), [](const BlockData &a, const BlockData &b) { return a.id < b.id; });
  for (const auto &blockDataEntry : prelimData) {
    data.push_back(blockDataEntry);
  }
}

