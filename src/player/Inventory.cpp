//
// Created by Tony Adriansen on 12/3/23.
//

#include "Inventory.h"

void Inventory::scrollHotbar(bool scrollUp) {
    int current = heldBlock;
    int direction = scrollUp ? 1 : -1;
    int next = current + direction;

    if (next > 15) {
        next = 1;
    } else if (next < 1) {
        next = 15;
    }
    heldBlock = static_cast<Block::ID>(next);
}

Inventory::Inventory() {
    for (int i = 0; i < Block::BLOCK_COUNT; i++) {
        blocks[static_cast<Block::ID>(i)] = 3000;
    }
}

Block::ID Inventory::getHeldBlock() {
    return heldBlock;
}
