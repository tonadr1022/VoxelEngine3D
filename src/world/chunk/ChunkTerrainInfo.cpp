//
// Created by Tony Adriansen on 1/1/2024.
//

#include "ChunkTerrainInfo.hpp"
#include "../generation/TerrainGenerator.hpp"
#include "../chunk/Chunk.hpp"

ChunkTerrainInfo::ChunkTerrainInfo(glm::ivec2 pos, int seed)
    : m_pos(pos), m_seed(seed) {
}

void ChunkTerrainInfo::generateTerrainData() {
  auto chunkWorldPos = m_pos * CHUNK_SIZE;

  HeightMap heightMap{};
  TerrainGenerator::getHeightMap(chunkWorldPos, m_seed, heightMap);
  m_heightMap = heightMap;

  TreeMap treeMap{};
  TerrainGenerator::getTreeMap(chunkWorldPos, m_seed, treeMap);
  m_treeMap = treeMap;

  TerrainGenerator::generateTerrain(heightMap, m_blocks, m_numBlocksPlaced);
  m_done = true;
}

void ChunkTerrainInfo::applyTerrainDataToChunk(Chunk *(&chunks)[CHUNKS_PER_STACK]) {
  for (int z = 0; z < CHUNKS_PER_STACK; z++) {
    int zOffset0 = z * CHUNK_VOLUME;
    int zOffset1 = zOffset0 + CHUNK_VOLUME;
    std::copy(m_blocks + zOffset0, m_blocks + zOffset1, chunks[z]->m_blocks);
    chunks[z]->chunkState = ChunkState::TERRAIN_GENERATED;
    chunks[z]->m_numNonAirBlocks = m_numBlocksPlaced[z];
  }
}