//
// Created by Tony Adriansen on 1/16/2024.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_GENERATION_BIOME_HPP_
#define VOXEL_ENGINE_SRC_WORLD_GENERATION_BIOME_HPP_

#include "../block/Block.hpp"
#include "json/json.hpp"

class Chunk;
class Biome {
 public:
  explicit Biome(nlohmann::json biomeData);
  [[nodiscard]] inline Block getSurfaceBlock() const {return m_surfaceBlock;}
  void buildStructure(glm::ivec2 pos, Chunk *chunk, float noise);

 private:
  std::vector<float> m_structureFrequencies;
  int m_numStructureTypes;
  Block m_surfaceBlock;
};
#endif //VOXEL_ENGINE_SRC_WORLD_GENERATION_BIOME_HPP_
