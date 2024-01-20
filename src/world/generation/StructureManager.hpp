//
// Created by Tony Adriansen on 1/19/24.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_GENERATION_STRUCTUREMANAGER_HPP_
#define VOXEL_ENGINE_SRC_WORLD_GENERATION_STRUCTUREMANAGER_HPP_

#include "../../EngineConfig.hpp"
#include "BlockAndPos.hpp"
#include "Structure.hpp"

struct StructureIdAndFreq {
  int id;
  float frequency;

  StructureIdAndFreq(int pid, float pfrequency)
  : frequency(pfrequency), id(pid) {}
};

class StructureManager {
 public:
  void loadStructureData();
  std::vector<StructureIdAndFreq> getStructuresForBiome(nlohmann::json &biomeData);
  std::vector<BlockAndPos> getStructureBlocks(int structureId);
 private:
  static const std::unordered_map<std::string, TreeShape> JsonKeyToTreeShape;
  static const std::unordered_map<std::string, TreeType> JsonKeyToTreeType;
  static const std::unordered_map<std::string, TreeSize> JsonKeyToTreeSize;
  std::unordered_map<int, std::unique_ptr<Structure>> m_structureDataMap;
};

#endif //VOXEL_ENGINE_SRC_WORLD_GENERATION_STRUCTUREMANAGER_HPP_
