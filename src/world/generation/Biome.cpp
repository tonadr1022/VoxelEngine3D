//
// Created by Tony Adriansen on 1/16/2024.
//

#include "Biome.hpp"
#include "../block/BlockDB.hpp"

void Biome::buildStructure(glm::ivec2 pos, Chunk *chunk, float noise) {

}
Biome::Biome(nlohmann::json biomeData)
: m_structureFrequencies(biomeData["frequencies"].get<std::vector<float>>()),
m_numStructureTypes(static_cast<int>(m_structureFrequencies.size())),
m_surfaceBlock(BlockDB::blockIdFromName(biomeData["surfaceBlock"].get<std::string>())) {}

