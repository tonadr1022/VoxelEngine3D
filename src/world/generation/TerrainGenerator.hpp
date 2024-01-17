//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_TERRAINGENERATOR_HPP
#define VOXEL_ENGINE_TERRAINGENERATOR_HPP

#include "../../EngineConfig.hpp"
#include "../block/Block.hpp"
#include "../../AppConstants.hpp"
#include "../chunk/Chunk.hpp"
#include "Biome.hpp"

enum class BiomeValue {
  OCEAN,
  BEACH,
  DESERT,
  PLAINS,
  TUNDRA,
  FOREST,
  BOREAL_FOREST,
  JUNGLE,
};

using BiomeMap = std::array<BiomeValue, CHUNK_AREA>;

class TerrainGenerator {
 public:
  explicit TerrainGenerator(int seed);
  static void generateStructures(Chunk *chunk, HeightMap &heightMap, TreeMap &treeMap);

  void fillHeightMap(const glm::ivec2 &startWorldPos, HeightMap &result) const;
  void fillTreeMap(const glm::ivec2 &startWorldPos, TreeMap &result) const;

  void fillSimplexMaps(const glm::ivec2 &startWorldPos,
                       HeightMap &heightMap,
                       HeightMapFloats &heightMapFloats,
                       PrecipitationMap &precipitationMap,
                       TemperatureMap &temperatureMap) const;
  void generateTerrain(HeightMap &heightMap,
                              BiomeMap &biomeMap,
                              Block (&blocks)[CHUNK_VOLUME * CHUNKS_PER_STACK],
                              int (&numBlocksPlaced)[CHUNKS_PER_STACK]) const;

  static void makeTree(const glm::ivec3 &pos, Chunk *chunk);

  static inline int WORLD_HEIGHT_XYZ(int x, int y, int z) {
    return x + (y << 5) + (z << 10);
  }

 [[nodiscard]] const Biome &getBiome(BiomeValue biomeValue) const {
    return *m_biomeFetchMap.at(biomeValue);
  };

 private:
  int m_seed;
  TundraBiome m_tundraBiome;
  DesertBiome m_desertBiome;
  JungleBiome m_jungleBiome;
  ForestBiome m_forestBiome;
  BeachBiome m_beachBiome;
  OceanBiome m_oceanBiome;
  PlainsBiome m_plainsBiome;
  BorealForestBiome m_borealForestBiome;

  std::unordered_map<BiomeValue, Biome *> m_biomeFetchMap = {
      {BiomeValue::TUNDRA, &m_tundraBiome},
      {BiomeValue::DESERT, &m_desertBiome},
      {BiomeValue::JUNGLE, &m_jungleBiome},
      {BiomeValue::FOREST, &m_forestBiome},
      {BiomeValue::BEACH, &m_beachBiome},
      {BiomeValue::OCEAN, &m_oceanBiome},
      {BiomeValue::PLAINS, &m_plainsBiome},
      {BiomeValue::BOREAL_FOREST, &m_borealForestBiome},
  };
};

#endif //VOXEL_ENGINE_TERRAINGENERATOR_HPP
