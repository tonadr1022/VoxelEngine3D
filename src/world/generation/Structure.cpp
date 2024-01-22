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
//
//  int col = rand() % 4;
//  Block glowstoneColor;
//  switch (col) {
//    case 0:glowstoneColor = Block::GLOWSTONE_GREEN;
//      break;
//    case 1:glowstoneColor = Block::GLOWSTONE_RED;
//      break;
//    case 2:glowstoneColor = Block::GLOWSTONE_BLUE;
//      break;
//    default:glowstoneColor = Block::GLOWSTONE;
//  }
//  blocks.emplace_back(pos, glowstoneColor);

//  for (int z = NORMAL_HEIGHT_MAX; z < NORMAL_HEIGHT_MAX + 3; z++) {
//    for (int y = -2; y <= 2; y++) {
//      for (int x = -2; x <= 2; x++) {
//        blocks.push_back({{x, y, z}, leafType});
//      }
//    }
//  }
//
//  for (int z = NORMAL_HEIGHT_MAX + 3; z < NORMAL_HEIGHT_MAX + 5; z++) {
//        blocks.push_back({{0,0, z}, leafType});
//        blocks.push_back({{-1,-1, z}, leafType});
//        blocks.push_back({{1,1, z}, leafType});
//        blocks.push_back({{-1,1, z}, leafType});
//        blocks.push_back({{1,-1, z}, leafType});
//  }



  return blocks;
}
