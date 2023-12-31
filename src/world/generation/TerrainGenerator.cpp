//
// Created by Tony Adriansen on 11/16/23.
//

#include "TerrainGenerator.hpp"
#include "../chunk/Chunk.hpp"

void TerrainGenerator::generateStructures() {
  auto chunkWorldPos = m_chunk4.m_worldPos;
  std::array<int, CHUNK_AREA> heightMap{};
  getHeightMap(chunkWorldPos, m_seed, heightMap);

  FastNoiseSIMD *fastNoise = FastNoiseSIMD::NewFastNoiseSIMD();
  fastNoise->SetSeed(m_seed);
  fastNoise->SetFrequency(1.0f);
  // fastnoise vals are from -1 to 1
  float *treeMap = fastNoise->GetWhiteNoiseSet(chunkWorldPos.x, chunkWorldPos.y, 0, CHUNK_WIDTH,
                                               CHUNK_WIDTH, 1);

  int heightMapIndex = 0;
  for (int x = 0; x < CHUNK_WIDTH; x++) {
    for (int y = 0; y < CHUNK_WIDTH; y++) {
      int height = heightMap[heightMapIndex];
      // + 1 since tree map vals are [-1,1]
      // 1 / 100 prob for now
      if (m_chunk4.getBlock(x, y, height) != Block::GRASS) {
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

  m_chunk4.chunkState = ChunkState::FULLY_GENERATED;
}

void TerrainGenerator::makeTree(const glm::ivec3 &pos) {
  for (int i = 0; i < 10; i++) {
    setBlock(pos.x, pos.y, i + pos.z, Block::OAK_WOOD);
  }
  // leaves
  for (int x = -2; x <= 2; x++) {
    for (int y = -2; y <= 2; y++) {
      for (int z = 8; z <= 12; z++) {
        setBlock(pos.x + x,
                 pos.y + y,
                 pos.z + z,
                 Block::OAK_LEAVES);
      }
    }
  }
}

TerrainGenerator::TerrainGenerator(Chunk &chunk0,
                                   Chunk &chunk1,
                                   Chunk &chunk2,
                                   Chunk &chunk3,
                                   Chunk &chunk4,
                                   Chunk &chunk5,
                                   Chunk &chunk6,
                                   Chunk &chunk7,
                                   Chunk &chunk8, int seed)
    : m_chunk0(chunk0), m_chunk1(chunk1), m_chunk2(chunk2), m_chunk3(chunk3),
      m_chunk4(chunk4), m_chunk5(chunk5), m_chunk6(chunk6), m_chunk7(chunk7),
      m_chunk8(chunk8), m_seed(seed) {

}
void TerrainGenerator::setBlock(int x, int y, int z, Block block) {

  // adj block in -1, -1 chunk (Chunk 0)
  if (x < 0 && y < 0) {
    m_chunk0.setBlock(CHUNK_WIDTH + x, CHUNK_WIDTH + y, z, block);
  }
    // adj block in -1, 1 chunk (Chunk 2)
  else if (x < 0 && y >= CHUNK_WIDTH) {
    m_chunk2.setBlock(CHUNK_WIDTH + x, 0, z, block);
  }

    // adj block in 1, -1 chunk (Chunk 6)
  else if (x >= CHUNK_WIDTH && y < 0) {
    m_chunk6.setBlock(0, CHUNK_WIDTH + y, z, block);
  }

    // adj block in 1, 1 chunk (Chunk 8)
  else if (x >= CHUNK_WIDTH && y >= CHUNK_WIDTH) {
    m_chunk8.setBlock(0, 0, z, block);
  }

    // adj block in -1, 0 chunk (Chunk 1)
  else if (x < 0) {
    m_chunk1.setBlock(CHUNK_WIDTH + x, y, z, block);
  }

    // adj block in 1, 0 chunk (Chunk 7)
  else if (x >= CHUNK_WIDTH) {
    m_chunk7.setBlock(0, y, z, block);
  }

    // adj block in 0, -1 chunk (Chunk 3)
  else if (y < 0) {
    m_chunk3.setBlock(x, CHUNK_WIDTH + y, z, block);
  }

    // adj block in 0, 1 chunk (Chunk 5)
  else if (y >= CHUNK_WIDTH) {
    m_chunk5.setBlock(x, 0, z, block);
  }
    // in middle chunk
  else {
    m_chunk4.setBlock(x, y, z, block);
  }
}
void TerrainGenerator::getHeightMap(glm::ivec2 startWorldPos, int seed, std::array<int, CHUNK_AREA> &result) {
  FastNoiseSIMD *fastNoise = FastNoiseSIMD::NewFastNoiseSIMD();
  fastNoise->SetSeed(seed);
  fastNoise->SetFractalOctaves(4);
  fastNoise->SetFrequency(1.0f / 300.0f);
  float *heightMap = fastNoise->GetSimplexFractalSet(startWorldPos.x, startWorldPos.y, 0, CHUNK_WIDTH,
                                                     CHUNK_WIDTH, 1);
  int highest = 0;
  for (int i = 0; i < CHUNK_AREA; i++) {
    result[i] = (int) floor(heightMap[i] * 64) + 70;
    highest = std::max(highest, result[i]);
  }
}

void TerrainGenerator::generateTerrain(const glm::ivec2 &chunkWorldPos, int seed,  Block (&blocks)[CHUNK_VOLUME]) {
  std::array<int, CHUNK_AREA> heightMap{};
  TerrainGenerator::getHeightMap(chunkWorldPos,seed, heightMap);

  int heightMapIndex = 0;
  int z;
  for (int x = 0; x < CHUNK_WIDTH; x++) {
    for (int y = 0; y < CHUNK_WIDTH; y++) {
      int maxBlockHeight = heightMap[heightMapIndex];
      for (z = 0; z < maxBlockHeight - 4; z++) {
        blocks[XYZ(x, y, z)] = Block::STONE;
      }
      for (z = maxBlockHeight - 4; z < maxBlockHeight; z++) {
        blocks[XYZ(x, y, z)] = Block::DIRT;
      }
      if (maxBlockHeight <= 66) {
        blocks[XYZ(x, y, maxBlockHeight)] = Block::SAND;
      } else {
        blocks[XYZ(x, y, maxBlockHeight)] = Block::GRASS;
      }

      for (z = maxBlockHeight + 1; z <= 64; z++) {
        blocks[XYZ(x, y, z)] = Block::WATER;
      }

      heightMapIndex++;
    }
  }

}
