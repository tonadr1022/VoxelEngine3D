//
// Created by Tony Adriansen on 11/16/23.
//

#include "TerrainGenerator.hpp"
#include "../chunk/Chunk.hpp"

void TerrainGenerator::generateStructures(HeightMap &heightMap, TreeMap &treeMap) {
  auto chunkWorldPos = m_chunks[13]->m_worldPos;
  int chunkBaseZ = chunkWorldPos.z;

  int heightMapIndex = 0;
  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
      int height = heightMap[heightMapIndex] - chunkBaseZ;
      if (height < 0 || height >= CHUNK_SIZE) continue;
      // + 1 since tree map vals are [-1,1]
      // 1 / 100 prob for now
      if (m_chunks[13]->getBlock(x, y, height) != Block::GRASS) {
        heightMapIndex++;
        continue;
      }
      bool structureExists = static_cast<int>((treeMap[heightMapIndex] + 1) * 100.0f) == 0;
      if (structureExists) {
        makeTree({x, y, height + 1});
      }
      heightMapIndex++;
    }
  }
  m_chunks[13]->chunkState = ChunkState::STRUCTURES_GENERATED;
}

void TerrainGenerator::makeTree(const glm::ivec3 &pos) {
  for (int i = 0; i < 8; i++) {
    setBlock(pos.x, pos.y, i + pos.z, Block::OAK_WOOD);
  }
  // leaves
  for (int x = -2; x <= 2; x++) {
    for (int y = -2; y <= 2; y++) {
      for (int z = 8; z <= 12; z++) {
        setBlock(pos.x + x, pos.y + y, pos.z + z, Block::OAK_LEAVES);
      }
    }
  }
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
  setBlock(pos.x, pos.y, 8 + pos.z,glowstoneColor);
}

TerrainGenerator::TerrainGenerator(Chunk *(chunks)[27], int seed)
    : m_seed(seed) {
  for (int i = 0; i < 27; i++) {
    m_chunks[i] = chunks[i];
  }
}

void TerrainGenerator::setBlock(int x, int y, int z, Block block) {
  int offsetX = Utils::chunkNeighborOffset(x);
  int offsetY = Utils::chunkNeighborOffset(y);
  int offsetZ = Utils::chunkNeighborOffset(z);
  int localX = Utils::getLocalIndex(x);
  int localY = Utils::getLocalIndex(y);
  int localZ = Utils::getLocalIndex(z);
  m_chunks[Utils::getNeighborArrayIndex(offsetX, offsetY, offsetZ)]->setBlock(localX, localY, localZ, block);
}

void TerrainGenerator::getHeightMap(const glm::ivec2 &startWorldPos, int seed, HeightMap &result) {
  FastNoiseSIMD *fastNoise = FastNoiseSIMD::NewFastNoiseSIMD();
  fastNoise->SetSeed(seed);
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

void TerrainGenerator::generateTerrain(HeightMap &heightMap,
                                       Block (&blocks)[CHUNK_VOLUME * CHUNKS_PER_STACK],
                                       int (&numBlocksPlaced)[CHUNKS_PER_STACK]) {
  auto setBlockTerrain = [&](int x, int y, int z, Block block) {
    blocks[WORLD_HEIGHT_XYZ(x, y, z)] = block;
    numBlocksPlaced[z / CHUNK_SIZE]++;
  };

  int heightMapIndex = 0;
  int z;
  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
      int maxBlockHeight = heightMap[heightMapIndex];
      for (z = 0; z < maxBlockHeight - 4; z++) {
        setBlockTerrain(x, y, z, Block::STONE);
      }
      if (maxBlockHeight - 4 >= 0) {
        for (z = maxBlockHeight - 4; z < maxBlockHeight; z++) {
          setBlockTerrain(x, y, z, Block::DIRT);
        }
      }
      if (maxBlockHeight <= 66) {
        setBlockTerrain(x, y, maxBlockHeight, Block::SAND);
      } else {
        setBlockTerrain(x, y, maxBlockHeight, Block::GRASS);
      }

      for (z = maxBlockHeight + 1; z <= 64; z++) {
        setBlockTerrain(x, y, z, Block::WATER);
      }
      heightMapIndex++;
    }
  }
  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
      for (int z = 0; z < 15; z++) {
        setBlockTerrain(x, y, z, Block::STONE);
      }
    }
  }

//  setBlockTerrain(0, 0, 15, Block::GLOWSTONE_GREEN);
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

void TerrainGenerator::getTreeMap(const glm::ivec2 &startWorldPos, int seed, TreeMap &result) {
  FastNoiseSIMD *fastNoise = FastNoiseSIMD::NewFastNoiseSIMD();
  fastNoise->SetSeed(seed);
  fastNoise->SetFrequency(1.0f);
  // fastnoise vals are from -1 to 1
  float *treeMap = fastNoise->GetWhiteNoiseSet(startWorldPos.x, startWorldPos.y, 0, CHUNK_SIZE,
                                               CHUNK_SIZE, 1);

  std::copy(treeMap, treeMap + CHUNK_AREA, result.begin());
  FastNoiseSIMD::FreeNoiseSet(treeMap);
  delete fastNoise;

}
