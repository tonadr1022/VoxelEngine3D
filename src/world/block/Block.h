//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_BLOCK_H
#define VOXEL_ENGINE_BLOCK_H

#include "../../Config.h"

struct Block {
    enum ID : uint8_t {
        AIR = 0,
        DIRT,
        GRASS,
        STONE,
        WATER,
        SAND,
        WOOD,
        LEAVES,
        CACTUS,
        SNOW,
        ICE,
        BEDROCK,
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
