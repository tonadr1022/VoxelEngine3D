//
// Created by Tony Adriansen on 11/16/23.
//

#include "TerrainGenerator.hpp"
#include "../chunk/Chunk.hpp"
#include "../chunk/ChunkManager.hpp"

void TerrainGenerator::generateStructures() {
//  for (int x = 0; x < CHUNK_WIDTH; x++) {
//    for (int y = 0; y < CHUNK_WIDTH; y++) {
//
//
//    }
//  }
  makeTree({14, 14, 200});

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
        setBlock( pos.x + x,
                  pos.y + y,
                 pos.z + z,
                 Block::OAK_LEAVES);
      }
    }
  }

}
//void TerrainGenerator::generateStructuresFor(ChunkManager &chunkManager,
//                                             const Ref<Chunk> &chunk) {
//  for (int x = 0; x < CHUNK_WIDTH; x++) {
//    for (int y = 0; y < CHUNK_WIDTH; y++) {
////            int height = chunk->getMaxTerrainHeightAt(x, y);
//      int height = 200;
//      if (chunk->getBlock(x, y, height) == Block::GRASS) {
//        float r = ((float) rand() / (RAND_MAX));
//        if (r > 0.995f) {
//          makeTree(chunkManager, chunk,
//                   {x, y, height + 1});
//        }
//      }
//    }
//  }
//  chunk->chunkState = ChunkState::FULLY_GENERATED;
//}

//void TerrainGenerator::makeTree(ChunkManager &chunkManager,
//                                const Ref<Chunk> &chunk,
//                                const glm::ivec3 &position) {
//    glm::ivec2 chunkLocation = chunk->getLocation();
//    for (int i = 0; i < 10; i++) {
//        chunkManager.setBlock(
//                {chunkLocation.x + position.x, chunkLocation.y + position.y, position.z + i},
//                Block::OAK_WOOD);
//    }
//    // leaves
//    for (int x = -2; x <= 2; x++) {
//        for (int y = -2; y <= 2; y++) {
//            for (int z = 8; z <= 12; z++) {
//                chunkManager.setBlock(
//                        {chunkLocation.x + position.x + x, chunkLocation.y + position.y + y,
//                         position.z + z},
//                        Block::OAK_LEAVES);
//            }
//        }
//    }
//}

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
