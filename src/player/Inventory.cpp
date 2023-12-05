//
// Created by Tony Adriansen on 12/3/23.
//

#include "Inventory.h"

void Inventory::shiftHotbarSelectedItem(bool scrollUp) {
    int current = m_hotbarSelectedItemIndex;
    int direction = scrollUp ? 1 : -1;
    int next = current + direction;

    if (next > HOTBAR_SIZE) {
        next = 0;
    } else if (next < 0) {
        next = HOTBAR_SIZE;
    }
    m_hotbarSelectedItemIndex = next;
}

Inventory::Inventory(bool creative) {
    if (creative) {
        for (int i = 0; i < Block::BLOCK_COUNT; i++) {
            m_items[i] = ItemStack(static_cast<Block::ID>(i), ItemStack::MAX_STACK_SIZE);
        }
        for (int i = Block::BLOCK_COUNT; i < MAX_INVENTORY_SIZE; i++) {
            m_items[i] = ItemStack(Block::UNDEFINED, 0);
        }
    }
}

Block::ID Inventory::getHeldItem() {
    return m_items[m_hotbarSelectedItemIndex + m_hotbarStartIndex].getBlockId();
}


void Inventory::shiftHotbarStartIndex(bool shiftUp) {
    int current = m_hotbarStartIndex;
    int offset = shiftUp ? 10 : -10;
    int next = current + offset;

    if (next > MAX_INVENTORY_SIZE - HOTBAR_SIZE) {
        next = 0;
    } else if (next < 0) {
        next = MAX_INVENTORY_SIZE - HOTBAR_SIZE;
    }
    m_hotbarStartIndex = next;
}
