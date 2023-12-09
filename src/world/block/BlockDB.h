//
// Created by Tony Adriansen on 11/22/23.
//

#ifndef VOXEL_ENGINE_BLOCKDB_H
#define VOXEL_ENGINE_BLOCKDB_H

#include "Block.h"
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

struct BlockData {
    Block::ID id;
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
    static void loadData(const std::string& filePath);
    static BlockData& getBlockData(Block::ID id);
private:
    static std::unordered_map<Block::ID, BlockData> data;
};


#endif //VOXEL_ENGINE_BLOCKDB_H
