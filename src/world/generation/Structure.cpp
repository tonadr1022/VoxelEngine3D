//
// Created by Tony Adriansen on 1/19/24.
//

#include "Structure.hpp"

std::vector<BlockAndPos> Tree::build() {
  std::vector<BlockAndPos> blocks;
  glm::ivec3 pos = {0, 0, 0};
  if (size == TreeSize::Giant) {
    for (pos.x = 0; pos.x <= GIANT_WIDTH; pos.x++) {
      for (pos.y = 0; pos.y <= GIANT_WIDTH; pos.y++) {
        for (pos.z = 0; pos.z < NORMAL_HEIGHT_MAX; pos.z++) {
          blocks.emplace_back(pos, woodType);
        }
      }
    }
  } else {
    for (pos.z = 0; pos.z < NORMAL_HEIGHT_MAX; pos.z++) {
      blocks.emplace_back(pos, woodType);
    }
  }

  int radius = 6;
  for (int z = -radius; z < radius; z++) {
    for (int y = -radius; y < radius; y++) {
      for (int x = -radius; x < radius; x++) {
        if (x * x + y * y + z * z < radius * radius) {
          blocks.push_back({{x, y, z + NORMAL_HEIGHT_MAX}, leafType});
        }
      }
    }
  }

  return blocks;
}
