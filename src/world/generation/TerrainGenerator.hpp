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
  SPRUCE_FOREST,
  JUNGLE,
};

using BiomeMap = std::array<BiomeValue, CHUNK_AREA>;

class TerrainGenerator {
 public:
  explicit TerrainGenerator(int seed);
  void init();

  static void generateStructures(Chunk *chunk, HeightMap &heightMap, TreeMap &treeMap);

  void fillHeightMap(const glm::ivec2 &startWorldPos, HeightMap &result) const;
  void fillTreeMap(const glm::ivec2 &startWorldPos, TreeMap &result) const;

  void fillSimplexMaps(const glm::ivec2 &startWorldPos,
                       HeightMap &heightMap,
                       HeightMapFloats &heightMapFloats,
                       PrecipitationMap &precipitationMap,
                       TemperatureMap &temperatureMap) const;

  void fillTerrainMaps(glm::ivec2 startWorldPosRes, SimplexFloatArray &continentalnessRes,
                       SimplexFloatArray &erosionRes, SimplexFloatArray &peaksAndValleysRes,
                       SimplexFloatArray &temperatureRes, SimplexFloatArray &precipitationRes) const;

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

  [[nodiscard]] float heightFromContinentalness(float continentalness) const;

  void generateBiomeAndHeightMaps(HeightMap &heightMap,
                                  BiomeMap &biomeMap,
                                  SimplexFloatArray &continentalness,
                                  SimplexFloatArray &erosion,
                                  SimplexFloatArray &peaksAndValleys,
                                  SimplexFloatArray &temperature,
                                  SimplexFloatArray &precipitation);

 private:
  int m_seed;
  void initializeSplines();

  static inline float lerp(float x, glm::vec2 p1, glm::vec2 p2) {
    return p1.y + (x - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
  }

  float OceanMax = -0.4;
  float OceanToCoastMax = -0.3;
  float CoastMax = -0.2;
  float CoastHeight = 80;
  float OceanHeight = 50.0;
  std::vector<glm::vec2> m_continentalnessSplines;
//  std::array<glm::vec2, 10> CONTINENTALNESS_SPLINES = {
//      {{-1, OceanHeight}, {OceanMax, OceanHeight}, {OceanToCoastMax, CoastHeight}, {1.0, BaseTerrainHeight}}
//  };


  static constexpr float CONTINENTALNESS_FREQ = 1.0f / 1200.0f;
  static constexpr int CONTINENTALNESS_NUM_OCTAVES = 4;
  static constexpr float EROSION_FREQ = 1.0f / 1200.0f;
  static constexpr int EROSION_NUM_OCTAVES = 1;
  static constexpr float PEAKS_AND_VALLEYS_FREQ = 1.0f / 400.0f;
  static constexpr int PEAKS_AND_VALLEYS_NUM_OCTAVES = 3;
  static constexpr float TEMPERATURE_FREQ = 1.0f / 1000.0f;
  static constexpr int TEMPERATURE_NUM_OCTAVES = 2;
  static constexpr float PRECIPITATION_FREQ = 1.0f / 1000.0f;
  static constexpr int PRECIPITATION_NUM_OCTAVES = 1;

  TundraBiome m_tundraBiome;
  DesertBiome m_desertBiome;
  JungleBiome m_jungleBiome;
  ForestBiome m_forestBiome;
  BeachBiome m_beachBiome;
  OceanBiome m_oceanBiome;
  PlainsBiome m_plainsBiome;
  SpruceForestBiome m_spruceForestBiome;

  std::unordered_map<BiomeValue, Biome *> m_biomeFetchMap = {
      {BiomeValue::TUNDRA, &m_tundraBiome},
      {BiomeValue::DESERT, &m_desertBiome},
      {BiomeValue::JUNGLE, &m_jungleBiome},
      {BiomeValue::FOREST, &m_forestBiome},
      {BiomeValue::BEACH, &m_beachBiome},
      {BiomeValue::OCEAN, &m_oceanBiome},
      {BiomeValue::PLAINS, &m_plainsBiome},
      {BiomeValue::SPRUCE_FOREST, &m_spruceForestBiome},
  };
};

#endif //VOXEL_ENGINE_TERRAINGENERATOR_HPP
