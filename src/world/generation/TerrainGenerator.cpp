//
// Created by Tony Adriansen on 11/16/23.
//

#include "TerrainGenerator.hpp"
#include "json/json.hpp"
#include "../../utils/JsonUtils.hpp"

void TerrainGenerator::generateStructures(std::array<Chunk *, CHUNKS_PER_STACK> &chunks,
                                          HeightMap &heightMap,
                                          BiomeMap &biomeMap) const {
//  int chunkBaseZ = chunk->m_worldPos.z;
//  StructureFloatMap structureFloatMap;
//  fillTreeMap({chunks[0]->m_worldPos.x, chunks[0]->m_worldPos.y}, structureFloatMap);
//  int x, y, worldHeight, localHeight;
//  for (int i = 0; i < CHUNK_AREA; i++) {
//    x = i & 31;
//    y = (i >> 5) & 31;
//    worldHeight = heightMap[i];
//    localHeight = Utils::getLocalIndex(worldHeight);
//    const Biome &biome = getBiome(biomeMap[i]);
//    biome.buildStructure({x, y, localHeight + 1}, chunks[worldHeight / CHUNK_SIZE], structureFloatMap[i]);
//  }

  for (auto &chunk : chunks) {
    chunk->chunkState = ChunkState::STRUCTURES_GENERATED;
  }
}

void TerrainGenerator::makeTree(const glm::ivec3 &pos, Chunk *chunk) {
  for (int i = 1; i < 4; i++) {
    chunk->setBlockIncludingNeighborsOptimized({pos.x, pos.y, i + pos.z}, Block::OAK_WOOD);
  }
  // leaves
  for (int x = -2; x <= 2; x++) {
    for (int y = -2; y <= 2; y++) {
      for (int z = 4; z <= 8; z++) {
        glm::ivec3 blockPos = {pos.x + x, pos.y + y, pos.z + z};
        chunk->setBlockIncludingNeighborsOptimized(blockPos, Block::OAK_LEAVES);
      }
    }
  }
  int col = rand() % 4;
  Block glowstoneColor;
  switch (col) {
    case 0:glowstoneColor = Block::GLOWSTONE_GREEN;
      break;
    case 1:glowstoneColor = Block::GLOWSTONE_RED;
      break;
    case 2:glowstoneColor = Block::GLOWSTONE_BLUE;
      break;
    default:glowstoneColor = Block::GLOWSTONE;
  }
  chunk->setBlockIncludingNeighborsOptimized({pos.x, pos.y, pos.z}, glowstoneColor);
}

TerrainGenerator::TerrainGenerator(int seed, nlohmann::json biomeData)
    : m_seed(seed),
      m_plainsBiome(biomeData["plains"], m_structureManager),
      m_beachBiome(biomeData["beach"], m_structureManager),
      m_desertBiome(biomeData["desert"], m_structureManager),
      m_jungleBiome(biomeData["jungle"], m_structureManager),
      m_forestBiome(biomeData["forest"], m_structureManager),
      m_oceanBiome(biomeData["ocean"], m_structureManager),
      m_spruceForestBiome(biomeData["spruce_forest"], m_structureManager),
      m_tundraBiome(biomeData["tundra"], m_structureManager) {}

void TerrainGenerator::init() {
  loadSplineData();
  m_structureManager.loadStructureData();
}

void TerrainGenerator::generateTerrain(HeightMap &heightMap,
                                       BiomeMap &biomeMap,
                                       Block (&blocks)[CHUNK_VOLUME * CHUNKS_PER_STACK],
                                       int (&numBlocksPlaced)[CHUNKS_PER_STACK]) const {
  auto setBlockTerrain = [&](int x, int y, int z, Block block) {
    blocks[WORLD_HEIGHT_XYZ(x, y, z)] = block;
    numBlocksPlaced[z / CHUNK_SIZE]++;
  };

  int heightMapIndex = 0;
  int x, y, z;
//  for (y = 0; y < CHUNK_SIZE; y++) {
//    for (x = 0; x < CHUNK_SIZE; x++) {
//      int maxBlockHeight = heightMap[heightMapIndex];
//      for (z = 0; z < maxBlockHeight - 4; z++) {
//        setBlockTerrain(x, y, z, Block::STONE);
//      }
//      if (maxBlockHeight - 4 >= 0) {
//        for (z = maxBlockHeight - 4; z < maxBlockHeight; z++) {
//          setBlockTerrain(x, y, z, Block::DIRT);
//        }
//      }
//      // set surface block at max block height
//      setBlockTerrain(x, y, maxBlockHeight, getBiome(biomeMap[heightMapIndex]).getSurfaceBlock());
////      if (maxBlockHeight <= 66) {
////        setBlockTerrain(x, y, maxBlockHeight, Block::SAND);
////      } else {
////        setBlockTerrain(x, y, maxBlockHeight, Block::GRASS);
////      }
//
//      for (z = maxBlockHeight + 1; z <= 64; z++) {
//        setBlockTerrain(x, y, z, Block::WATER);
//      }
//      heightMapIndex++;
//    }
//  }

  for (x = 4; x < 15; x++) {
    for (y = 4; y < 15; y++) {
      for (z = 0; z < 32; z++) {
        setBlockTerrain(x, y, z, Block::STONE);
      }
//      setBlockTerrain(x, y, 38, Block::BEDROCK);
    }
  }
////
//  for (x = 2; x < CHUNK_SIZE-2; x++) {
//    for (y = 2; y < CHUNK_SIZE-2; y++) {
//
//      setBlockTerrain(x, y, 36, Block::BEDROCK);
//    }
//  }
//
//  setBlockTerrain(0, 0, 32, Block::GLOWSTONE_RED);
//  setBlockTerrain(7,7, 32, Block::GLOWSTONE_RED);
//  setBlockTerrain(15, 16, 15, Block::GLOWSTONE_RED);
//  setBlockTerrain(16, 15, 15, Block::GLOWSTONE_BLUE);

//  int maxBlockHeightAtx10y10 = heightMap[32 * 10 + 10];
//  setBlockTerrain(10, 10, maxBlockHeightAtx10y10, Block::GLOWSTONE_RED);
//
//  int maxBlockHeightAtx15y15 = heightMap[32 * 15 + 15];
//  setBlockTerrain(15, 15, maxBlockHeightAtx15y15, Block::GLOWSTONE_GREEN);
//
//  int maxBlockHeightAtx20y20 = heightMap[32 * 20 + 20];
//  setBlockTerrain(20, 20, maxBlockHeightAtx20y20, Block::GLOWSTONE_BLUE);
}

void TerrainGenerator::fillTreeMap(const glm::ivec2 &startWorldPos, StructureFloatMap &result) const {
  FastNoiseSIMD *fastNoise = FastNoiseSIMD::NewFastNoiseSIMD();
  fastNoise->SetSeed(m_seed);
  fastNoise->SetFrequency(1.0f);
  // fastnoise vals are from -1 to 1
  float *treeMap = fastNoise->GetWhiteNoiseSet(startWorldPos.y, startWorldPos.x, 0, CHUNK_SIZE,
                                               CHUNK_SIZE, 1);

  std::copy(treeMap, treeMap + CHUNK_AREA, result.begin());
  for (auto &num : result) {
    num = abs(num);
  }
  FastNoiseSIMD::FreeNoiseSet(treeMap);
  delete fastNoise;
}

float TerrainGenerator::getContinentalnessValue(float continentalness) const {
  for (size_t i = 0; i < m_continentalnessSplinePoints.size() - 1; i++) {
    if (continentalness <= m_continentalnessSplinePoints[i + 1].x) {
      return (lerp(continentalness, m_continentalnessSplinePoints[i], m_continentalnessSplinePoints[i + 1]) + 1) * 50;
    }
  }
  throw std::runtime_error("spline error");
}

void TerrainGenerator::fillTerrainMaps(glm::ivec2 startWorldPos,
                                       SimplexFloatArray &continentalnessRes,
                                       SimplexFloatArray &erosionRes,
                                       SimplexFloatArray &peaksAndValleysRes,
                                       SimplexFloatArray &temperatureRes,
                                       SimplexFloatArray &precipitationRes) const {
  FastNoiseSIMD *fastNoise = FastNoiseSIMD::NewFastNoiseSIMD();
  fastNoise->SetSeed(m_seed);

  fastNoise->SetFractalOctaves(CONTINENTALNESS_NUM_OCTAVES);
  fastNoise->SetFrequency(CONTINENTALNESS_FREQ);
  float *continentalness = fastNoise->GetSimplexFractalSet(startWorldPos.y, startWorldPos.x, 0, CHUNK_SIZE,
                                                           CHUNK_SIZE, 1);
  std::copy(continentalness, continentalness + CHUNK_AREA, continentalnessRes.begin());
  FastNoiseSIMD::FreeNoiseSet(continentalness);

  fastNoise->SetFractalOctaves(EROSION_NUM_OCTAVES);
  fastNoise->SetFrequency(EROSION_FREQ);
  float *erosion = fastNoise->GetSimplexFractalSet(startWorldPos.y, startWorldPos.x, 0, CHUNK_SIZE,
                                                   CHUNK_SIZE, 1);
  std::copy(erosion, erosion + CHUNK_AREA, erosionRes.begin());
  FastNoiseSIMD::FreeNoiseSet(erosion);

  fastNoise->SetFractalOctaves(PEAKS_AND_VALLEYS_NUM_OCTAVES);
  fastNoise->SetFrequency(PEAKS_AND_VALLEYS_FREQ);
  float *peaksAndValleys = fastNoise->GetSimplexFractalSet(startWorldPos.y, startWorldPos.x, 0, CHUNK_SIZE,
                                                           CHUNK_SIZE, 1);
  std::copy(peaksAndValleys, peaksAndValleys + CHUNK_AREA, peaksAndValleysRes.begin());
  FastNoiseSIMD::FreeNoiseSet(peaksAndValleys);

  fastNoise->SetFractalOctaves(TEMPERATURE_NUM_OCTAVES);
  fastNoise->SetFrequency(TEMPERATURE_FREQ);
  float *precipitation = fastNoise->GetSimplexFractalSet(startWorldPos.y, startWorldPos.x, 0, CHUNK_SIZE,
                                                         CHUNK_SIZE, 1);
  std::copy(precipitation, precipitation + CHUNK_AREA, precipitationRes.begin());
  FastNoiseSIMD::FreeNoiseSet(precipitation);

  fastNoise->SetFractalOctaves(PRECIPITATION_NUM_OCTAVES);
  fastNoise->SetFrequency(PRECIPITATION_FREQ);
  float *temperature = fastNoise->GetSimplexFractalSet(startWorldPos.y, startWorldPos.x, 0, CHUNK_SIZE,
                                                       CHUNK_SIZE, 1);
  std::copy(temperature, temperature + CHUNK_AREA, temperatureRes.begin());
  FastNoiseSIMD::FreeNoiseSet(temperature);

  delete fastNoise;

}

void TerrainGenerator::generateBiomeAndHeightMaps(HeightMap &heightMap,
                                                  BiomeMap &biomeMap,
                                                  SimplexFloatArray &continentalness,
                                                  SimplexFloatArray &erosion,
                                                  SimplexFloatArray &peaksAndValleys,
                                                  SimplexFloatArray &temperature,
                                                  SimplexFloatArray &precipitation) {

}

void TerrainGenerator::loadSplineData() {
  nlohmann::json splineData = JsonUtils::openJson("resources/terrain/terrain.json");
  nlohmann::json continentalnessPoints = splineData["continentalness"]["points"];
  for (const auto &point : continentalnessPoints) {
    m_continentalnessSplinePoints.emplace_back(point["location"].get<float>(), point["value"].get<float>());
  }
//  for (const auto &point : continentalnessPoints) {
//    m_continentalnessSplineX.emplace_back(point["location"].get<float>());
//    m_continentalnessSplineY.emplace_back(point["value"].get<float>());
//  }
//
//  m_continentalnessSpline.set_boundary(tk::spline::first_deriv, 0.0, tk::spline::first_deriv, 0.0);
//  m_continentalnessSpline.set_points(m_continentalnessSplineX, m_continentalnessSplineY, tk::spline::cspline_hermite);
}
