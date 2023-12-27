//
// Created by Tony Adriansen on 11/16/23.
//

#include "TerrainGenerator.hpp"
#include "../chunk/Chunk.hpp"
#include "../chunk/ChunkManager.hpp"

void TerrainGenerator::generateTerrainFor(const Ref<Chunk> &chunk) {


    FastNoiseSIMD *fastNoise = FastNoiseSIMD::NewFastNoiseSIMD();
    fastNoise->SetSeed(m_seed);
    fastNoise->SetFractalOctaves(4);
    fastNoise->SetFrequency(1.0f / 300.0f);
    float *heightMap = fastNoise->GetSimplexFractalSet(chunk->m_pos.x, chunk->m_pos.y, 0,
                                                       CHUNK_WIDTH, CHUNK_WIDTH, 1);

    int heights[CHUNK_AREA];
    int highest = 0;

    for (int i = 0; i < CHUNK_AREA; i++) {
        heights[i] = (int) floor(heightMap[i] * 64) + 100;
        highest = std::max(highest, heights[i]);
//        chunk->m_maxTerrainHeights[i] = heights[i];
    }

    for (int z = 0; z <= highest; z++) {
        int heightMapIndex = 0;
        for (int x = 0; x < CHUNK_WIDTH; x++) {
            for (int y = 0; y < CHUNK_WIDTH; y++) {
                int height = heights[heightMapIndex];
                if (z < height) {
                    chunk->setBlock(x, y, z, Block::DIRT);
                } else if (z == height) {
                    chunk->setBlock(x, y, z, Block ::GRASS);
                }
                heightMapIndex++;
            }
        }
    }


    chunk->chunkState = ChunkState::TERRAIN_GENERATED;
}

void TerrainGenerator::generateStructuresFor(ChunkManager &chunkManager, const Ref<Chunk> &chunk) {
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_WIDTH; y++) {
//            int height = chunk->getMaxTerrainHeightAt(x, y);
            int height = 200;
            if (chunk->getBlock(x, y, height) == Block::GRASS) {
                float r = ((float) rand() / (RAND_MAX));
                if (r > 0.995f) {
                    makeTree(chunkManager, chunk,
                             {x, y, height + 1});
                }
            }
        }
    }
    chunk->chunkState = ChunkState::FULLY_GENERATED;
}

void TerrainGenerator::makeTree(ChunkManager &chunkManager, const Ref<Chunk> &chunk,
                                const glm::ivec3 &position) {
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
}

TerrainGenerator::TerrainGenerator(int seed) : m_seed(seed) {}
