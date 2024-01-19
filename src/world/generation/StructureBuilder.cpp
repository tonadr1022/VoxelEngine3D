//
// Created by Tony Adriansen on 1/19/24.
//

#include "StructureBuilder.hpp"
std::vector<BlockAndPos> StructureBuilder::buildBasicTree(Block woodType,
                                                          Block leafType,
                                                          int height,
                                                          int leafDiameter,
                                                          bool spherical) {
  std::vector<BlockAndPos> blocks;
  glm::ivec3 pos = {0, 0, 0};
  for (int i = 0; i < height; i++) {
    pos.z = i;
    blocks.emplace_back(pos, woodType);
  }
  int radius = leafDiameter / 2;
  if (spherical) {
    for (int z = -radius; z < radius; z++) {
      for (int y = -radius; y < radius; y++) {
        for (int x = -radius; x < radius; x++) {
          if (x*x + y*y + z*z < radius * radius) {
            blocks.push_back({{x,y,z + height}, leafType});
          }
        }
      }
    }
  } else {

  }

  return blocks;
}
