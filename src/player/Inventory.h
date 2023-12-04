//
// Created by Tony Adriansen on 12/3/23.
//

#ifndef VOXEL_ENGINE_INVENTORY_H
#define VOXEL_ENGINE_INVENTORY_H


#include "../world/block/Block.h"

class Inventory {
public:
    Inventory();

    void scrollHotbar(bool scrollUp);

    void selectBlock(Block::ID block);

    Block::ID getHeldBlock();


private:
    std::map<Block::ID, int> blocks;
    Block::ID heldBlock = Block::ID::DIRT;
};


#endif //VOXEL_ENGINE_INVENTORY_H
