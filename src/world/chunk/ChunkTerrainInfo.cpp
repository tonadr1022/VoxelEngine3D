//
// Created by Tony Adriansen on 1/5/2024.
//

#include "ChunkTerrainInfo.hpp"

ChunkTerrainInfo::ChunkTerrainInfo(glm::ivec2 pos, TerrainGenerator &terrainGenerator)
    : m_pos(pos), m_terrainGenerator(terrainGenerator) {
}

void ChunkTerrainInfo::generateTerrainData() {
  auto chunkWorldPos = m_pos * CHUNK_SIZE;


  SimplexFloatArray continentalness;
  SimplexFloatArray erosion;
  SimplexFloatArray peaksAndValleys;
  SimplexFloatArray temperature;
  SimplexFloatArray precipitation;

  m_terrainGenerator.fillTerrainMaps(chunkWorldPos,
                                     continentalness,
                                     erosion,
                                     peaksAndValleys,
                                     temperature,
                                     precipitation);
  HeightMap heightMap;
  m_terrainGenerator.generateBiomeAndHeightMaps(heightMap,
                                                m_biomeMap,
                                                continentalness,
                                                erosion,
                                                peaksAndValleys,
                                                temperature,
                                                precipitation);

  for (int i = 0; i < CHUNK_AREA; i++) {
    heightMap[i] = (int) m_terrainGenerator.getContinentalnessValue(continentalness[i]);
  }

  m_heightMap = heightMap;

//  m_precipitationMap = precipitation;
//  m_temperatureMap = temperature;

  float precipMapVal, tempMapVal, continentalnessVal;
  int x, y;
  for (int i = 0; i < CHUNK_AREA; i++) {
    x = i & 31;
    y = (i >> 5) & 31;
    continentalnessVal = continentalness[i];
    precipMapVal = precipitation[i] + 1;
    tempMapVal = temperature[i] + 1;
    if (continentalnessVal <= -0.4) {
      m_biomeMap[i] = BiomeValue::OCEAN;
    } else if (continentalnessVal < -0.2) {
      m_biomeMap[i] = BiomeValue::BEACH;
    } else {
      if (precipMapVal < 1.0) {
        if (tempMapVal < 0.3) {
          m_biomeMap[i] = BiomeValue::TUNDRA;
        } else if (tempMapVal < 1.2) {
          m_biomeMap[i] = BiomeValue::PLAINS;
        } else {
          m_biomeMap[i] = BiomeValue::DESERT;
        }
      } else {
        if (tempMapVal < 0.8) {
          m_biomeMap[i] = BiomeValue::SPRUCE_FOREST;
        } else if (tempMapVal < 1.4) {
          m_biomeMap[i] = BiomeValue::FOREST;
        } else {
          m_biomeMap[i] = BiomeValue::JUNGLE;
        }
      }
    }

    if (m_biomeMap[i] >= BiomeValue::NUM_BIOMES) {
      std::cout << "here, error\n";
    }
  }

  m_terrainGenerator.generateTerrain(heightMap, m_biomeMap, m_blocks, m_numBlocksPlaced);
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