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
    static BlockData& getBlockData(Block id);
private:
    static std::vector<BlockData> data;
};


#endif //VOXEL_ENGINE_BLOCKDB_HPP
