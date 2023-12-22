//
// Created by Tony Adriansen on 12/4/23.
//

#include "ItemStack.hpp"
#include "Inventory.hpp"

Block ItemStack::getBlockId() const {
    return m_block;
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

void ItemStack::setBlock(Block block) {
    ItemStack::m_block = block;
}

void ItemStack::clear() {
    m_block = Block::AIR;
    m_quantity = 0;
}

ItemStack::ItemStack(Block block, int quantity) : m_block(block), m_quantity(quantity) {}

ItemStack::ItemStack() {
    m_block = Block::AIR;
    m_quantity = 0;
}


