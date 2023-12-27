//
// Created by Tony Adriansen on 12/3/23.
//

#ifndef VOXEL_ENGINE_INVENTORY_HPP
#define VOXEL_ENGINE_INVENTORY_HPP

#include "../world/block/Block.hpp"
#include "ItemStack.hpp"
#include "../EngineConfig.hpp"

class Inventory {
 public:
  explicit Inventory(bool creative = true);

  void shiftHotbarSelectedItem(bool scrollUp);
  void shiftHotbarStartIndex(bool shiftUp);
  Block getHeldItem();

  static constexpr int HOTBAR_SIZE = 10;
  static constexpr int MAX_INVENTORY_SIZE = 100;
 private:
  std::array<ItemStack, MAX_INVENTORY_SIZE> m_items;
  int m_hotbarSelectedItemIndex = 0;
  int m_hotbarStartIndex = 0;
};

#endif //VOXEL_ENGINE_INVENTORY_HPP
