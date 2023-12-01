//
// Created by Tony Adriansen on 11/16/23.
//

#include "Chunklet.h"

Chunklet::Chunklet(glm::ivec3 location) : location(location) {
}

void Chunklet::setBlock(int x, int y, int z, Block block) {
    blocks[getIndex(x, y, z)] = block;
}

Block Chunklet::getBlock(int x, int y, int z) {
    return blocks[getIndex(x, y, z)];
}

int Chunklet::getIndex(int x, int y, int z) {
    return y * CHUNK_AREA + z * CHUNK_WIDTH + x;
}
