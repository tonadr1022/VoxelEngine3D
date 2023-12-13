//
// Created by Tony Adriansen on 11/22/23.
//

#include "BlockDB.hpp"
#include "json/json.hpp"
#include <fstream>
#include <filesystem>

std::unordered_map<Block::ID, BlockData> BlockDB::data;

void BlockDB::loadData(const std::string &filePath) {
    std::filesystem::path jsonPath = filePath + "block_data.json";
    std::ifstream jsonFile(jsonPath);
    if (!jsonFile.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath + "block_data.json");
    }
    nlohmann::json blocks;
    jsonFile >> blocks;
    jsonFile.close();
    for (const auto &[blockName, blockData]: blocks.items()) {
        BlockData blockDataEntry{};
        blockDataEntry.name = blockName;
        blockDataEntry.id = static_cast<Block::ID>(blockData["id"].get<int>());
        blockDataEntry.topTexCoords.x = blockData["topTexCoords"][0].get<int>();
        blockDataEntry.topTexCoords.y = blockData["topTexCoords"][1].get<int>();
        blockDataEntry.frontTexCoords.x = blockData["frontTexCoords"][0].get<int>();
        blockDataEntry.frontTexCoords.y = blockData["frontTexCoords"][1].get<int>();
        blockDataEntry.backTexCoords.x = blockData["backTexCoords"][0].get<int>();
        blockDataEntry.backTexCoords.y = blockData["backTexCoords"][1].get<int>();
        blockDataEntry.leftTexCoords.x = blockData["leftTexCoords"][0].get<int>();
        blockDataEntry.leftTexCoords.y = blockData["leftTexCoords"][1].get<int>();
        blockDataEntry.rightTexCoords.x = blockData["rightTexCoords"][0].get<int>();
        blockDataEntry.rightTexCoords.y = blockData["rightTexCoords"][1].get<int>();
        blockDataEntry.bottomTexCoords.x = blockData["bottomTexCoords"][0].get<int>();
        blockDataEntry.bottomTexCoords.y = blockData["bottomTexCoords"][1].get<int>();
        blockDataEntry.isTransparent = blockData["isTransparent"].get<bool>();
        blockDataEntry.isCollidable = blockData["isCollidable"].get<bool>();
        data[blockDataEntry.id] = blockDataEntry;
    }
}

BlockData &BlockDB::getBlockData(Block::ID id) {
    return data[id];
}