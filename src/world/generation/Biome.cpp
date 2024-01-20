//
// Created by Tony Adriansen on 1/16/2024.
//

#include "Biome.hpp"
#include "../block/BlockDB.hpp"
#include "../chunk/Chunk.hpp"
#include "StructureManager.hpp"

void Biome::buildStructure(glm::ivec3 pos, Chunk *chunk, float noise) const {
  float accumulatedFrequency = 0;
  for (const auto &idAndFreq : m_structuresAndFrequencies) {
    accumulatedFrequency += idAndFreq.frequency;
    if (noise <= accumulatedFrequency) {
      std::vector<BlockAndPos> blockAndPositions = m_structureManager.getStructureBlocks(idAndFreq.id);
      for (const auto &bp : blockAndPositions) {
        chunk->setBlockIncludingNeighborsOptimized(pos + bp.pos, bp.block);
      }
      break;
    }
  }
}

Biome::Biome(nlohmann::json biomeData, StructureManager &structureManager)
    : m_structuresAndFrequencies(structureManager.getStructuresForBiome(biomeData)),
      m_surfaceBlock(BlockDB::blockIdFromName(biomeData["surfaceBlock"].get<std::string>())),
      m_structureManager(structureManager) {}

