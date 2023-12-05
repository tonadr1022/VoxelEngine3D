//
// Created by Tony Adriansen on 12/4/23.
//

#include "ItemStack.h"
#include "Inventory.h"

Block::ID ItemStack::getBlockId() const {
    return m_blockId;
}

int ItemStack::getQuantity() const {
    return m_quantity;
}

void ItemStack::setQuantity(int quantity) {
    ItemStack::m_quantity = quantity;
}

void ItemStack::addQuantity(int quantity) {
    ItemStack::m_quantity += quantity;
}

void ItemStack::removeQuantity(int quantity) {
    ItemStack::m_quantity -= quantity;
}

bool ItemStack::isEmpty() const {
    return m_quantity == 0;
}

bool ItemStack::isFull() const {
    return m_quantity == MAX_STACK_SIZE;
}

void ItemStack::setBlockId(Block::ID blockId) {
    ItemStack::m_blockId = blockId;
}

void ItemStack::clear() {
    m_blockId = Block::ID::AIR;
    m_quantity = 0;
}

ItemStack::ItemStack(Block::ID blockId, int quantity) : m_blockId(blockId), m_quantity(quantity) {}

ItemStack::ItemStack() {
    m_blockId = Block::ID::AIR;
    m_quantity = 0;
}


