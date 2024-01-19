//
// Created by Tony Adriansen on 1/16/2024.
//

#include "Biome.hpp"
#include "../block/BlockDB.hpp"
#include "../chunk/Chunk.hpp"
#include "StructureBuilder.hpp"

void Biome::buildStructure(glm::ivec3 pos, Chunk *chunk, float noise) const {
  bool structureExists = static_cast<int>((noise + 1) * 100.0f) == 0;
  if (structureExists) {
    std::vector<BlockAndPos> blocks = StructureBuilder::buildBasicTree(Block::OAK_WOOD, Block::OAK_LEAVES, 15, 12, true);
    for (auto &bp : blocks) {
      chunk->setBlockIncludingNeighborsOptimized(pos + bp.pos, bp.block);
    }
  }
}

Biome::Biome(nlohmann::json biomeData)
    : m_structureFrequencies(biomeData["frequencies"].get<std::vector<float>>()),
      m_numStructureTypes(static_cast<int>(m_structureFrequencies.size())),
      m_surfaceBlock(BlockDB::blockIdFromName(biomeData["surfaceBlock"].get<std::string>())) {}

