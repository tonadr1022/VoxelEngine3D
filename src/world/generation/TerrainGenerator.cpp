//
// Created by Tony Adriansen on 11/16/23.
//

#include "TerrainGenerator.hpp"

void TerrainGenerator::generateStructures(Chunk *chunk, HeightMap &heightMap, TreeMap &treeMap) {
  int chunkBaseZ = chunk->m_worldPos.z;

  int heightMapIndex = 0;
  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      int height = (int) floor((heightMap[heightMapIndex]) * 64) - chunkBaseZ;
//      int height = heightMap[heightMapIndex] - chunkBaseZ;
      if (height < 0 || height >= CHUNK_SIZE) continue;
      // + 1 since tree map vals are [-1,1]
      // 1 / 100 prob for now
      if (chunk->getBlock(x, y, height) != Block::GRASS) {
        heightMapIndex++;
        continue;
      }
      bool structureExists = static_cast<int>((treeMap[heightMapIndex] + 1) * 100.0f) == 0;
      if (structureExists) {
        makeTree({x, y, height + 1}, chunk);
      }
      heightMapIndex++;
    }
  }
  chunk->chunkState = ChunkState::STRUCTURES_GENERATED;
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

TerrainGenerator::TerrainGenerator(int seed) : m_seed(seed) {

}

void TerrainGenerator::fillHeightMap(const glm::ivec2 &startWorldPos, HeightMap &result) const {
  FastNoiseSIMD *fastNoise = FastNoiseSIMD::NewFastNoiseSIMD();
  fastNoise->SetSeed(m_seed);
  fastNoise->SetFractalOctaves(4);
  fastNoise->SetFrequency(1.0f / 300.0f);
  float *heightMap = fastNoise->GetSimplexFractalSet(startWorldPos.x, startWorldPos.y, 0, CHUNK_SIZE,
                                                     CHUNK_SIZE, 1);
  for (int i = 0; i < CHUNK_AREA; i++) {
    result[i] = (int) floor((heightMap[i] + 1) * 64) + 1;
  }
  FastNoiseSIMD::FreeNoiseSet(heightMap);
  delete fastNoise;
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
    for (y = 0; y < CHUNK_SIZE; y++) {
      for (x = 0; x < CHUNK_SIZE; x++) {
      int maxBlockHeight = heightMap[heightMapIndex];
      for (z = 0; z < maxBlockHeight - 4; z++) {
        setBlockTerrain(x, y, z, Block::STONE);
      }
      if (maxBlockHeight - 4 >= 0) {
        for (z = maxBlockHeight - 4; z < maxBlockHeight; z++) {
          setBlockTerrain(x, y, z, Block::DIRT);
        }
      }
      // set surface block at max block height
      setBlockTerrain(x, y, maxBlockHeight, getBiome(biomeMap[heightMapIndex]).getSurfaceBlock());
//      if (maxBlockHeight <= 66) {
//        setBlockTerrain(x, y, maxBlockHeight, Block::SAND);
//      } else {
//        setBlockTerrain(x, y, maxBlockHeight, Block::GRASS);
//      }

      for (z = maxBlockHeight + 1; z <= 64; z++) {
        setBlockTerrain(x, y, z, Block::WATER);
      }
      heightMapIndex++;
    }
  }

//  for (x = 0; x < CHUNK_SIZE; x++) {
//    for (y = 0; y < CHUNK_SIZE; y++) {
//      for (z = 0; z < 32; z++) {
//        setBlockTerrain(x, y, z, Block::STONE);
//      }
////      setBlockTerrain(x, y, 38, Block::BEDROCK);
//    }
//  }
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

void TerrainGenerator::fillTreeMap(const glm::ivec2 &startWorldPos, TreeMap &result) const {
  FastNoiseSIMD *fastNoise = FastNoiseSIMD::NewFastNoiseSIMD();
  fastNoise->SetSeed(m_seed);
  fastNoise->SetFrequency(1.0f);
  // fastnoise vals are from -1 to 1
  float *treeMap = fastNoise->GetWhiteNoiseSet(startWorldPos.y, startWorldPos.x, 0, CHUNK_SIZE,
                                               CHUNK_SIZE, 1);

  std::copy(treeMap, treeMap + CHUNK_AREA, result.begin());
  FastNoiseSIMD::FreeNoiseSet(treeMap);
  delete fastNoise;
}

void TerrainGenerator::fillSimplexMaps(const glm::ivec2 &startWorldPos, HeightMap &heightMap, HeightMapFloats &heightMapFloats,
                                       PrecipitationMap &precipitationMap, TemperatureMap &temperatureMap) const {
  FastNoiseSIMD *fastNoise = FastNoiseSIMD::NewFastNoiseSIMD();
  fastNoise->SetSeed(m_seed);
  fastNoise->SetFractalOctaves(4);
  fastNoise->SetFrequency(1.0f / 300.0f);
  float *hmFloats = fastNoise->GetSimplexFractalSet(startWorldPos.y, startWorldPos.x, 0, CHUNK_SIZE,
                                                           CHUNK_SIZE, 1);
  for (int i = 0; i < CHUNK_AREA; i++) {
    heightMap[i] = (int) floor((hmFloats[i] + 1) * 64) + 1;
  }

  std::copy(hmFloats, hmFloats + CHUNK_AREA, heightMapFloats.begin());
  for (float &val : heightMapFloats) {
    val++;
  }
  FastNoiseSIMD::FreeNoiseSet(hmFloats);

  fastNoise->SetFractalOctaves(2);
  fastNoise->SetFrequency(1.0f / 1000.0f);
  float *precipitationMapFloats = fastNoise->GetSimplexFractalSet(startWorldPos.y, startWorldPos.x, 0, CHUNK_SIZE,
                                                                  CHUNK_SIZE, 1);
  std::copy(precipitationMapFloats, precipitationMapFloats + CHUNK_AREA, precipitationMap.begin());
  for (float &val : precipitationMap) {
    val++;
  }
  FastNoiseSIMD::FreeNoiseSet(precipitationMapFloats);

  fastNoise->SetFractalOctaves(1);
  fastNoise->SetFrequency(1.0f / 1000.0f);
  float *temperatureMapFloats = fastNoise->GetSimplexFractalSet(startWorldPos.y, startWorldPos.x, 0, CHUNK_SIZE,
                                                                CHUNK_SIZE, 1);
  std::copy(temperatureMapFloats, temperatureMapFloats + CHUNK_AREA, temperatureMap.begin());
  for (float &val : temperatureMap) {
    val++;
  }
  FastNoiseSIMD::FreeNoiseSet(temperatureMapFloats);

  delete fastNoise;
}
