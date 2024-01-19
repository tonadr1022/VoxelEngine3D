//
// Created by Tony Adriansen on 1/19/24.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_GENERATION_STRUCTUREBUILDER_HPP_
#define VOXEL_ENGINE_SRC_WORLD_GENERATION_STRUCTUREBUILDER_HPP_

#include "../../EngineConfig.hpp"
#include "BlockAndPos.hpp"

class StructureBuilder {
 public:
  static std::vector<BlockAndPos> buildBasicTree(Block woodType, Block leafType, int height, int leafDiameter, bool spherical);

};

#endif //VOXEL_ENGINE_SRC_WORLD_GENERATION_STRUCTUREBUILDER_HPP_
