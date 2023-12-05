//
// Created by Tony Adriansen on 12/3/23.
//

#ifndef VOXEL_ENGINE_INVENTORY_H
#define VOXEL_ENGINE_INVENTORY_H


#include "../world/block/Block.h"
#include "ItemStack.h"

class Inventory {
public:
    explicit Inventory(bool creative = true);

    void shiftHotbarSelectedItem(bool scrollUp);

//    void selectBlock(Block::ID block);

    void shiftHotbarStartIndex(bool shiftUp);

    Block::ID getHeldItem();

    static constexpr int HOTBAR_SIZE = 10;
    static constexpr int MAX_INVENTORY_SIZE = 100;
private:
    std::array<ItemStack, MAX_INVENTORY_SIZE> m_items;
    int m_hotbarSelectedItemIndex = 0; // index of item in hotbar 0-9
    int m_hotbarStartIndex = 0; // 0, 10, 20, 30, 40, 50, 60, 70, 80, 90
};


#endif //VOXEL_ENGINE_INVENTORY_H
