//
// Created by Tony Adriansen on 11/22/23.
//

#include "BlockDB.h"
#include <fstream>

const std::array<std::string, 20> blockFileNames{
        "air",
        "bedrock",
        "cobblestone",
        "crafting_table",
        "diamond_block",
        "dirt",
        "glowstone",
        "grass",
        "gravel",
        "ice_block",
        "jungle_wood",
        "oak_leaves",
        "oak_wood",
        "sand",
        "snowy_grass_block",
        "stone",
        "water",
};

std::unordered_map<Block::ID, BlockData> BlockDB::data;

void BlockDB::loadData(const std::string &filePath) {
    std::ifstream jsonFile(filePath + "block_data.json");
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
//    for (const std::string &blockFileName: blockFileNames) {
//        std::ifstream blockFile(filePath + blockFileName + ".txt");
//        if (!blockFile.is_open()) {
//            std::cerr << "Failed to open file: " << filePath << blockFileName << ".txt"
//                      << std::endl;
//            continue;
//        }
//        std::string line;
//        BlockData blockData{};
//        while (getline(blockFile, line)) {
//            if (line.empty() || line[0] == '#')
//                continue;
//            if (line == "Name") {
//                getline(blockFile, line);
//                blockData.name = line;
//            } else if (line == "Id") {
//                getline(blockFile, line);
//                blockData.id = static_cast<Block::ID>(std::stoi(line));
//            } else if (line == "TexAll") {
//                int x, y;
//                blockFile >> x >> y;
//                blockData.topTexCoords.x = x;
//                blockData.topTexCoords.y = y;
//                blockData.frontTexCoords.x = x;
//                blockData.frontTexCoords.y = y;
//                blockData.backTexCoords.x = x;
//                blockData.backTexCoords.y = y;
//                blockData.leftTexCoords.x = x;
//                blockData.leftTexCoords.y = y;
//                blockData.rightTexCoords.x = x;
//                blockData.rightTexCoords.y = y;
//                blockData.bottomTexCoords.x = x;
//                blockData.bottomTexCoords.y = y;
//            } else if (line == "TexTop") {
//                int x, y;
//                blockFile >> x >> y;
//                blockData.topTexCoords.x = x;
//                blockData.topTexCoords.y = y;
//            } else if (line == "TexSide") {
//                int x, y;
//                blockFile >> x >> y;
//                blockData.frontTexCoords.x = x;
//                blockData.frontTexCoords.y = y;
//                blockData.backTexCoords.x = x;
//                blockData.backTexCoords.y = y;
//                blockData.leftTexCoords.x = x;
//                blockData.leftTexCoords.y = y;
//                blockData.rightTexCoords.x = x;
//                blockData.rightTexCoords.y = y;
//            } else if (line == "TexFront") {
//                int x, y;
//                blockFile >> x >> y;
//                blockData.frontTexCoords.x = x;
//                blockData.frontTexCoords.y = y;
//            } else if (line == "TexBack") {
//                int x, y;
//                blockFile >> x >> y;
//                blockData.backTexCoords.x = x;
//                blockData.backTexCoords.y = y;
//            } else if (line == "TexLeft") {
//                int x, y;
//                blockFile >> x >> y;
//                blockData.leftTexCoords.x = x;
//                blockData.leftTexCoords.y = y;
//            } else if (line == "TexRight") {
//                int x, y;
//                blockFile >> x >> y;
//                blockData.rightTexCoords.x = x;
//                blockData.rightTexCoords.y = y;
//            } else if (line == "TexBottom") {
//                int x, y;
//                blockFile >> x >> y;
//                blockData.bottomTexCoords.x = x;
//                blockData.bottomTexCoords.y = y;
//            } else if (line == "Transparent") {
//                blockFile >> blockData.isTransparent;
//            } else if (line == "Collidable") {
//                blockFile >> blockData.isCollidable;
//            }
//        }
//        data[blockData.id] = blockData;
//    }

}

BlockData &BlockDB::getBlockData(Block::ID id) {
    return data[id];
}

void BlockDB::saveJson() {
//    nlohmann::json blocks;
//    for (const auto&[id, blockData]: data) {
//        blocks[blockData.name] = {
//                {"id", id},
//                {"topTexCoords", {blockData.topTexCoords.x, blockData.topTexCoords.y}},
//                {"frontTexCoords", {blockData.frontTexCoords.x, blockData.frontTexCoords.y}},
//                {"backTexCoords", {blockData.backTexCoords.x, blockData.backTexCoords.y}},
//                {"leftTexCoords", {blockData.leftTexCoords.x, blockData.leftTexCoords.y}},
//                {"rightTexCoords", {blockData.rightTexCoords.x, blockData.rightTexCoords.y}},
//                {"bottomTexCoords", {blockData.bottomTexCoords.x, blockData.bottomTexCoords.y}},
//                {"isTransparent", blockData.isTransparent},
//                {"isCollidable", blockData.isCollidable}
//        };
//    }
//    std::ofstream jsonFile("blocks.json");
//    jsonFile << std::setw(4) << blocks << std::endl;
//    jsonFile.close();

    std::ofstream jsonFile("block_data.json");
    jsonFile << "{\n";
    for (const auto &[id, blockData]: data) {
        jsonFile << "    \"" << blockData.name << "\": {\n";
        jsonFile << "        \"id\": " << static_cast<int>(id) << ",\n";
        jsonFile << "        \"topTexCoords\": [" << blockData.topTexCoords.x << ", "
                 << blockData.topTexCoords.y
                 << "],\n";
        jsonFile << "        \"frontTexCoords\": [" << blockData.frontTexCoords.x << ", "
                 << blockData.frontTexCoords.y
                 << "],\n";
        jsonFile << "        \"backTexCoords\": [" << blockData.backTexCoords.x << ", "
                 << blockData.backTexCoords.y
                 << "],\n";
        jsonFile << "        \"leftTexCoords\": [" << blockData.leftTexCoords.x << ", "
                 << blockData.leftTexCoords.y
                 << "],\n";
        jsonFile << "        \"rightTexCoords\": [" << blockData.rightTexCoords.x << ", "
                 << blockData.rightTexCoords.y
                 << "],\n";
        jsonFile << "        \"bottomTexCoords\": [" << blockData.bottomTexCoords.x << ", "
                 << blockData.bottomTexCoords.y
                 << "],\n";
        jsonFile << "        \"isTransparent\": " << (blockData.isTransparent ? "true" : "false")
                 << ",\n";
        jsonFile << "        \"isCollidable\": " << (blockData.isCollidable ? "true" : "false")
                 << "\n";
        jsonFile << "    },\n";
    }
    jsonFile << "}";
    jsonFile.close();


}