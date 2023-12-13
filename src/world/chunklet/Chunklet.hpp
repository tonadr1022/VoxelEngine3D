//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CHUNKLET_HPP
#define VOXEL_ENGINE_CHUNKLET_HPP


#include "../block/Block.hpp"
#include <glm/glm.hpp>
#include "../../AppConstants.hpp"
#include <array>


class Chunk;

class Chunklet {
public:
    Chunklet() = default;
    explicit Chunklet(glm::ivec3 location);
    void setBlock(int x, int y, int z, Block block);
    Block getBlock(int x, int y, int z);
    glm::ivec3 location{};
private:
    std::array<Block, CHUNKLET_VOLUME> blocks;
    static int getIndex(int x, int y, int z);

};


#endif //VOXEL_ENGINE_CHUNKLET_HPP
