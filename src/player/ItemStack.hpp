//
// Created by Tony Adriansen on 12/4/23.
//

#ifndef VOXEL_ENGINE_ITEMSTACK_HPP
#define VOXEL_ENGINE_ITEMSTACK_HPP


#include "../world/block/Block.hpp"

class ItemStack {
public:
    ItemStack(Block::ID blockId, int quantity);

    ItemStack();

    [[nodiscard]] Block::ID getBlockId() const;

    int getQuantity() const;

    void setQuantity(int quantity);

    void addQuantity(int quantity);

    void removeQuantity(int quantity);

    bool isEmpty() const;

    bool isFull() const;

    void setBlockId(Block::ID blockId);

    void clear();

    constexpr static int MAX_STACK_SIZE = 64;

private:
    Block::ID m_blockId;
    int m_quantity;


};


#endif //VOXEL_ENGINE_ITEMSTACK_HPP
