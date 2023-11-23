//
// Created by Tony Adriansen on 11/22/23.
//

#include "BlockDB.h"
#include <fstream>

const std::array<std::string, 4> blockFileNames{
        "air",
        "dirt",
        "grass",
//        "stone",
//        "water",
//        "sand",
        "wood",
//        "leaves",
//        "cactus",
//        "snow",
//        "ice",
//        "bedrock",
};

std::unordered_map<Block::ID, BlockData> BlockDB::data;

void BlockDB::loadData(const std::string &filePath) {
    for (const std::string &blockFileName: blockFileNames) {
        std::ifstream blockFile(filePath + blockFileName + ".txt");
        if (!blockFile.is_open()) {
            std::cerr << "Failed to open file: " << filePath << blockFileName << ".txt"
                      << std::endl;
            continue;
        }
        std::string line;
        BlockData blockData{};
        while (getline(blockFile, line)) {
            if (line.empty() || line[0] == '#')
                continue;
            if (line == "Name") {
                getline(blockFile, line);
                blockData.name = line;
            } else if (line == "Id") {
                getline(blockFile, line);
                blockData.id = static_cast<Block::ID>(std::stoi(line));
            } else if (line == "TexAll") {
                int x, y;
                blockFile >> x >> y;
                blockData.topTexCoords.x = x;
                blockData.topTexCoords.y = y;
                blockData.frontTexCoords.x = x;
                blockData.frontTexCoords.y = y;
                blockData.backTexCoords.x = x;
                blockData.backTexCoords.y = y;
                blockData.leftTexCoords.x = x;
                blockData.leftTexCoords.y = y;
                blockData.rightTexCoords.x = x;
                blockData.rightTexCoords.y = y;
                blockData.bottomTexCoords.x = x;
                blockData.bottomTexCoords.y = y;
            } else if (line == "TexTop") {
                int x, y;
                blockFile >> x >> y;
                blockData.topTexCoords.x = x;
                blockData.topTexCoords.y = y;
            } else if (line == "TexSide") {
                int x, y;
                blockFile >> x >> y;
                blockData.frontTexCoords.x = x;
                blockData.frontTexCoords.y = y;
                blockData.backTexCoords.x = x;
                blockData.backTexCoords.y = y;
                blockData.leftTexCoords.x = x;
                blockData.leftTexCoords.y = y;
                blockData.rightTexCoords.x = x;
                blockData.rightTexCoords.y = y;
            } else if (line == "TexFront") {
                int x, y;
                blockFile >> x >> y;
                blockData.frontTexCoords.x = x;
                blockData.frontTexCoords.y = y;
            } else if (line == "TexBack") {
                int x, y;
                blockFile >> x >> y;
                blockData.backTexCoords.x = x;
                blockData.backTexCoords.y = y;
            } else if (line == "TexLeft") {
                int x, y;
                blockFile >> x >> y;
                blockData.leftTexCoords.x = x;
                blockData.leftTexCoords.y = y;
            } else if (line == "TexRight") {
                int x, y;
                blockFile >> x >> y;
                blockData.rightTexCoords.x = x;
                blockData.rightTexCoords.y = y;
            } else if (line == "TexBottom") {
                int x, y;
                blockFile >> x >> y;
                blockData.bottomTexCoords.x = x;
                blockData.bottomTexCoords.y = y;
            } else if (line == "Transparent") {
                blockFile >> blockData.isTransparent;
            } else if (line == "Collidable") {
                blockFile >> blockData.isCollidable;
            }
        }
        data[blockData.id] = blockData;
    }
}

BlockData &BlockDB::getBlockData(Block::ID id) {
    return data[id];
}
