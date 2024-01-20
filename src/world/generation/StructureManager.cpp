//
// Created by Tony Adriansen on 1/19/24.
//

#include "StructureManager.hpp"
#include "json/json.hpp"
#include "../../utils/JsonUtils.hpp"
#include "../block/BlockDB.hpp"



std::vector<StructureIdAndFreq> StructureManager::getStructuresForBiome(nlohmann::json &biomeData) {
  std::vector<StructureIdAndFreq> res;
  for (const auto &tree : biomeData["structures"]["trees"]) {
    res.emplace_back(tree["id"].get<int>(), tree["frequency"].get<float>());
  }
  return res;
}

void StructureManager::loadStructureData() {
  nlohmann::json jsonData = JsonUtils::openJson("resources/terrain/structureData.json");
  for (const auto &treeData : jsonData["trees"]) {
    int id = treeData["id"].get<int>();
    TreeShape shape = JsonKeyToTreeShape.at(treeData["shape"].get<std::string>());
    Block leafType = BlockDB::blockIdFromName(treeData["leaf_type"].get<std::string>());
    Block woodType = BlockDB::blockIdFromName(treeData["wood_type"].get<std::string>());
    TreeSize size = JsonKeyToTreeSize.at(treeData["size"].get<std::string>());
    m_structureDataMap.emplace(id, std::make_unique<Tree>(shape, woodType, leafType, size));
  }
}

std::vector<BlockAndPos> StructureManager::getStructureBlocks(int structureId) {
  return m_structureDataMap.at(structureId)->build();
}

const std::unordered_map<std::string, TreeShape> StructureManager::JsonKeyToTreeShape = {
    {"basic", TreeShape::Basic},
    {"spherical", TreeShape::Spherical},
    {"conical", TreeShape::Conical},
    {"canopy", TreeShape::Canopy},
    {"palm", TreeShape::Palm},
};

const std::unordered_map<std::string, TreeType> StructureManager::JsonKeyToTreeType = {
    {"oak", TreeType::Oak},
    {"birch", TreeType::Birch},
    {"spruce", TreeType::Spruce},
    {"jungle", TreeType::Jungle},
    {"palm", TreeType::Palm},
};

const std::unordered_map<std::string, TreeSize> StructureManager::JsonKeyToTreeSize = {
    {"normal", TreeSize::Normal},
    {"giant", TreeSize::Giant}
};
