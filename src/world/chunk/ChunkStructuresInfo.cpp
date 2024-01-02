//
// Created by Tony Adriansen on 1/1/2024.
//

#include "ChunkStructuresInfo.hpp"
#include "../chunk/Chunk.hpp"

ChunkStructuresInfo::ChunkStructuresInfo(Chunk *(chunks)[27], int seed) : m_terrainGenerator(chunks, seed),
                                                                          m_pos(chunks[13]->m_pos) {

}

void ChunkStructuresInfo::generateStructureData(HeightMap &heightMap, TreeMap &treeMap) {
  m_terrainGenerator.generateStructures(heightMap, treeMap);
  m_done = true;
}