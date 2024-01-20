//
// Created by Tony Adriansen on 1/16/2024.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_GENERATION_BIOME_HPP_
#define VOXEL_ENGINE_SRC_WORLD_GENERATION_BIOME_HPP_

#include "../block/Block.hpp"
#include "json/json.hpp"
#include "StructureManager.hpp"

class Chunk;
class Biome {
 public:
  explicit Biome(nlohmann::json biomeData, StructureManager &structureBuilder);
  [[nodiscard]] inline Block getSurfaceBlock() const {return m_surfaceBlock;}
  void buildStructure(glm::ivec3 pos, Chunk *chunk, float noise) const;

 private:
  std::vector<StructureIdAndFreq> m_structuresAndFrequencies;
  Block m_surfaceBlock;
  StructureManager &m_structureManager;
};
#endif //VOXEL_ENGINE_SRC_WORLD_GENERATION_BIOME_HPP_
