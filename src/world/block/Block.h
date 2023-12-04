//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_BLOCK_H
#define VOXEL_ENGINE_BLOCK_H

#include "../../Config.h"

struct Block {
    enum ID : uint8_t {
        AIR = 0,
        DIRT, // 1
        GRASS, // 2
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
        BLOCK_COUNT,
        UNDEFINED
    };

    ID id;

    [[nodiscard]] bool isTransparent() const {
        return id == AIR || id == WATER;
    }
    Block() : id(UNDEFINED) {}
    explicit Block(ID id) : id(id) {}
};

#endif //VOXEL_ENGINE_BLOCK_H
