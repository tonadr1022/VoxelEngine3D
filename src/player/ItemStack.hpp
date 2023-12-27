//
// Created by Tony Adriansen on 12/4/23.
//

#ifndef VOXEL_ENGINE_ITEMSTACK_HPP
#define VOXEL_ENGINE_ITEMSTACK_HPP

#include "../world/block/Block.hpp"

class ItemStack {
 public:
  ItemStack(Block block, int quantity);
  ItemStack();

  [[nodiscard]] Block getBlockId() const;
  [[nodiscard]] int getQuantity() const;
  void setQuantity(int quantity);
  void addQuantity(int quantity);
  void removeQuantity(int quantity);
  [[nodiscard]] bool isEmpty() const;
  [[nodiscard]] bool isFull() const;
  void setBlock(Block block);
  void clear();

  constexpr static int MAX_STACK_SIZE = 64;

 private:
  Block m_block;
  int m_quantity;
};

#endif //VOXEL_ENGINE_ITEMSTACK_HPP
