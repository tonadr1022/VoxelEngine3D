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
#include "json/json.hpp"

enum class BiomeValue {
  OCEAN,
  BEACH,
  DESERT,
  PLAINS,
  TUNDRA,
  FOREST,
  SPRUCE_FOREST,
  JUNGLE,
  NUM_BIOMES
};

using BiomeMap = std::array<BiomeValue, CHUNK_AREA>;

class TerrainGenerator {
 public:
  explicit TerrainGenerator(int seed, nlohmann::json biomeData);
  void init();

  void generateStructures(std::array<Chunk *, CHUNKS_PER_STACK> &chunks,
                          HeightMap &heightMap,
                          BiomeMap &biomeMap) const;

  void fillTreeMap(const glm::ivec2 &startWorldPos, StructureFloatMap &result) const;
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

  [[nodiscard]] float getContinentalnessValue(float continentalness) const;

  void generateBiomeAndHeightMaps(HeightMap &heightMap,
                                  BiomeMap &biomeMap,
                                  SimplexFloatArray &continentalness,
                                  SimplexFloatArray &erosion,
                                  SimplexFloatArray &peaksAndValleys,
                                  SimplexFloatArray &temperature,
                                  SimplexFloatArray &precipitation);

 private:
  int m_seed;
  void loadSplineData();

  static inline float lerp(float x, glm::vec2 p1, glm::vec2 p2) {
    return p1.y + (x - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
  }

  std::vector<glm::vec2> m_continentalnessSplinePoints;
  std::vector<glm::vec2> m_peaksAndValleysSplines;

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

  Biome m_tundraBiome;
  Biome m_desertBiome;
  Biome m_jungleBiome;
  Biome m_forestBiome;
  Biome m_beachBiome;
  Biome m_oceanBiome;
  Biome m_plainsBiome;
  Biome m_spruceForestBiome;

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
