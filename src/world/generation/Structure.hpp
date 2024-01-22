//
// Created by Tony Adriansen on 1/19/24.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_GENERATION_STRUCTURE_HPP_
#define VOXEL_ENGINE_SRC_WORLD_GENERATION_STRUCTURE_HPP_

#include "../../EngineConfig.hpp"
#include "BlockAndPos.hpp"


enum class TreeShape {
  Basic,
  Spherical,
  Conical,
  Canopy,
  Palm
};

enum class TreeType {
  Oak,
  Birch,
  Spruce,
  Jungle,
  Palm
};

enum class TreeSize {
  Normal,
  Giant
};


class Structure {
 public:
  virtual std::vector<BlockAndPos> build() = 0;
};

class Tree : public Structure {
 public:
  Tree(TreeShape pShape, Block pWoodType, Block pLeafType, TreeSize pSize)
      : shape(pShape), woodType(pWoodType), leafType(pLeafType), size(pSize) {}

  TreeShape shape;
  Block woodType;
  Block leafType;
  TreeSize size;

  std::vector<BlockAndPos> build() override;

 private:
  static const int NORMAL_HEIGHT_MIN = 4;
  static const int NORMAL_HEIGHT_MAX = 8;
  static const int GIANT_HEIGHT_MIN = 12;
  static const int GIANT_HEIGHT_MAX = 40;
  static const int GIANT_WIDTH = 2;

};

#endif //VOXEL_ENGINE_SRC_WORLD_GENERATION_STRUCTURE_HPP_
