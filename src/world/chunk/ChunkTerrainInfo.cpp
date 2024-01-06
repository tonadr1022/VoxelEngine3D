//
// Created by Tony Adriansen on 1/5/2024.
//

#include "ChunkTerrainInfo.hpp"


ChunkTerrainInfo::ChunkTerrainInfo(glm::ivec2 pos, TerrainGenerator &terrainGenerator)
    : m_pos(pos), m_terrainGenerator(terrainGenerator) {
}

void ChunkTerrainInfo::generateTerrainData() {
  auto chunkWorldPos = m_pos * CHUNK_SIZE;

  HeightMap heightMap;
  m_terrainGenerator.getHeightMap(chunkWorldPos, heightMap);
  m_heightMap = heightMap;

  TreeMap treeMap;
  m_terrainGenerator.getTreeMap(chunkWorldPos, treeMap);
  m_treeMap = treeMap;

  TerrainGenerator::generateTerrain(heightMap, m_blocks, m_numBlocksPlaced);
  m_done = true;
}

void ChunkTerrainInfo::applyTerrainDataToChunks(Chunk *(&chunks)[CHUNKS_PER_STACK]) {
  for (int z = 0; z < CHUNKS_PER_STACK; z++) {
    int zOffset0 = z * CHUNK_VOLUME;
    int zOffset1 = zOffset0 + CHUNK_VOLUME;
    std::copy(m_blocks + zOffset0, m_blocks + zOffset1, chunks[z]->m_blocks);
    chunks[z]->chunkState = ChunkState::TERRAIN_GENERATED;
    chunks[z]->m_numNonAirBlocks = m_numBlocksPlaced[z];
  }
}