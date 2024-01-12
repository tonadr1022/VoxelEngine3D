//
// Created by Tony Adriansen on 11/16/23.
//

#include "TerrainGenerator.hpp"

void TerrainGenerator::generateStructures(Chunk *chunk, HeightMap &heightMap, TreeMap &treeMap) {
  int chunkBaseZ = chunk->m_worldPos.z;

  int heightMapIndex = 0;
  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
      int height = heightMap[heightMapIndex] - chunkBaseZ;
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
//  for (int i = 1; i < 4; i++) {
//    chunk->setBlockIncludingNeighborsOptimized({pos.x, pos.y, i + pos.z}, Block::OAK_WOOD);
//  }
//  // leaves
//  for (int x = -2; x <= 2; x++) {
//    for (int y = -2; y <= 2; y++) {
//      for (int z = 4; z <= 8; z++) {
//        glm::ivec3 blockPos = {pos.x + x, pos.y + y, pos.z + z};
//        chunk->setBlockIncludingNeighborsOptimized(blockPos, Block::OAK_LEAVES);
//      }
//    }
//  }
  int col = rand() % 4;
  Block glowstoneColor;
  switch(col) {
    case 0:
      glowstoneColor = Block::GLOWSTONE_GREEN;
      break;
    case 1:
      glowstoneColor = Block::GLOWSTONE_RED;
      break;
    case 2:
      glowstoneColor = Block::GLOWSTONE_BLUE;
      break;
    default:
      glowstoneColor = Block::GLOWSTONE;
  }
  chunk->setBlockIncludingNeighborsOptimized({pos.x, pos.y, pos.z},glowstoneColor);
}

TerrainGenerator::TerrainGenerator(int seed) : m_seed(seed) {}



void TerrainGenerator::getHeightMap(const glm::ivec2 &startWorldPos, HeightMap &result) const {
  FastNoiseSIMD *fastNoise = FastNoiseSIMD::NewFastNoiseSIMD();
  fastNoise->SetSeed(m_seed);
  fastNoise->SetFractalOctaves(4);
  fastNoise->SetFrequency(1.0f / 300.0f);
  float *heightMap = fastNoise->GetSimplexFractalSet(startWorldPos.x, startWorldPos.y, 0, CHUNK_SIZE,
                                                     CHUNK_SIZE, 1);
  int highest = 0;
  for (int i = 0; i < CHUNK_AREA; i++) {
    result[i] = (int) floor((heightMap[i] + 1) * 64) + 1;
    highest = std::max(highest, result[i]);
  }
  FastNoiseSIMD::FreeNoiseSet(heightMap);
  delete fastNoise;
}

void TerrainGenerator::generateTerrain(HeightMap &heightMap, Block (&blocks)[CHUNK_VOLUME * CHUNKS_PER_STACK], int (&numBlocksPlaced)[CHUNKS_PER_STACK]) {
  auto setBlockTerrain = [&](int x, int y, int z, Block block) {
    blocks[WORLD_HEIGHT_XYZ(x, y, z)] = block;
    numBlocksPlaced[z / CHUNK_SIZE]++;
  };

  int heightMapIndex = 0;
  int x,y,z;
//  for (x = 0; x < CHUNK_SIZE; x++) {
//    for (y = 0; y < CHUNK_SIZE; y++) {
//      int maxBlockHeight = heightMap[heightMapIndex];
//      for (z = 0; z < maxBlockHeight - 4; z++) {
//        setBlockTerrain(x, y, z, Block::STONE);
//      }
//      if (maxBlockHeight - 4 >= 0) {
//        for (z = maxBlockHeight - 4; z < maxBlockHeight; z++) {
//          setBlockTerrain(x, y, z, Block::DIRT);
//        }
//      }
//      if (maxBlockHeight <= 66) {
//        setBlockTerrain(x, y, maxBlockHeight, Block::SAND);
//      } else {
//        setBlockTerrain(x, y, maxBlockHeight, Block::GRASS);
//      }
//
//      for (z = maxBlockHeight + 1; z <= 64; z++) {
//        setBlockTerrain(x, y, z, Block::WATER);
//      }
//      heightMapIndex++;
//    }
//  }

  for (x = 0; x < CHUNK_SIZE; x++) {
    for (y = 0; y < CHUNK_SIZE; y++) {
      for (z = 0; z < 32; z++) {
        setBlockTerrain(x, y, z, Block::STONE);
      }
//      setBlockTerrain(x, y, 38, Block::BEDROCK);
    }
  }

  for (x = 2; x < CHUNK_SIZE-2; x++) {
    for (y = 2; y < CHUNK_SIZE-2; y++) {

      setBlockTerrain(x, y, 36, Block::BEDROCK);
    }
  }

  setBlockTerrain(0, 0, 32, Block::GLOWSTONE_RED);
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

void TerrainGenerator::getTreeMap(const glm::ivec2 &startWorldPos, TreeMap &result) const {
  FastNoiseSIMD *fastNoise = FastNoiseSIMD::NewFastNoiseSIMD();
  fastNoise->SetSeed(m_seed);
  fastNoise->SetFrequency(1.0f);
  // fastnoise vals are from -1 to 1
  float *treeMap = fastNoise->GetWhiteNoiseSet(startWorldPos.x, startWorldPos.y, 0, CHUNK_SIZE,
                                               CHUNK_SIZE, 1);

  std::copy(treeMap, treeMap + CHUNK_AREA, result.begin());
  FastNoiseSIMD::FreeNoiseSet(treeMap);
  delete fastNoise;

}
