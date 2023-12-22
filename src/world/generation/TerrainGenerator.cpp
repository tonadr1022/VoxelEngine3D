//
// Created by Tony Adriansen on 11/16/23.
//

#include "TerrainGenerator.hpp"
#include "../chunk/Chunk.hpp"
#include "../chunk/ChunkManager.hpp"
#include "../utils/Timer.hpp"

void TerrainGenerator::generateTerrainFor(const Ref<Chunk> &chunk) const {
//    Timer timer("Gen");
    glm::ivec2 chunkLocation = chunk->getLocation();

    FastNoiseSIMD *fastNoise = FastNoiseSIMD::NewFastNoiseSIMD();
    fastNoise->SetSeed(1);
    fastNoise->SetFractalOctaves(4);
    fastNoise->SetFrequency(0.002f);
    float *heightMap = fastNoise->GetSimplexFractalSet(chunkLocation.x, chunkLocation.y, 0,
                                                       CHUNK_WIDTH, CHUNK_WIDTH, 1);

    int heights[CHUNK_AREA];
    int highest = 0;

    for (int i = 0; i < CHUNK_AREA; i++) {
        heights[i] = (int) floor(heightMap[i] * 64) + 100;
        highest = std::max(highest, heights[i]);
    }

//    for (int z = 0; z <= CHUNK_HEIGHT; z++) {
//        int heightMapIndex = 0;
//        for (int x = 0; x < CHUNK_WIDTH; x++) {
//            for (int y = 0; y < CHUNK_WIDTH; y++) {
//                int height = heights[heightMapIndex];
//                if (z < height) {
//                    chunk->setBlock(x, y, z, Block(Block::DIRT));
//                } else if (z == height) {
//                    chunk->setBlock(x, y, z, Block(Block::GRASS));
//                }
//                heightMapIndex++;
//            }
//        }
//    }

for (int z = 0; z <= 100; z++) {
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_WIDTH; y++) {
            chunk->setBlock(x, y, z, Block(Block::OAK_WOOD));
        }
    }
}

    chunk->chunkState = ChunkState::TERRAIN_GENERATED;
}

void TerrainGenerator::generateStructuresFor(ChunkManager &chunkManager, const Ref<Chunk> &chunk) {
    glm::ivec2 chunkLocation = chunk->getLocation();

//    for (int x = 0; x < CHUNK_WIDTH; x++) {
//        for (int y = 0; y < CHUNK_WIDTH; y++) {
//            for (int z = 0; z < CHUNK_HEIGHT; z++) {
//                if (chunk->getBlock(x, y, z).id == Block::GRASS) {
//                    float r = ((float) rand() / (RAND_MAX));
//                    if (r > 0.995f) {
//                        makeTree(chunkManager, chunk,
//                                 {x, y, z + 1});
//                    }
//                }
//            }
//        }
//    }
//    for (int i = 0; i < 10; i++) {
//        chunk->setBlock(15, 15, height + i, Block(Block::OAK_WOOD));
////chunkManager.setBlock({chunkLocation.x + 15, chunkLocation.y + 14, height + i}, Block(Block::OAK_WOOD));
//    }

//    for (int x = 14; x <= 17; x++) {
//        for (int y = 14; y <= 17; y++) {
//            for (int z = height + 10; z <= height + 14; z++) {
//                chunkManager.setBlock({chunkLocation.x + x, chunkLocation.y + y, z},
//                                      Block(Block::OAK_LEAVES));
//            }
//        }
//    }
    chunk->chunkState = ChunkState::FULLY_GENERATED;
}

void TerrainGenerator::makeTree(ChunkManager &chunkManager, const Ref<Chunk> &chunk,
                                const glm::ivec3 &position) {
    glm::ivec2 chunkLocation = chunk->getLocation();
    for (int i = 0; i < 10; i++) {
        chunkManager.setBlock(
                {chunkLocation.x + position.x, chunkLocation.y + position.y, position.z + i},
                Block(Block::OAK_WOOD));
    }
    std::cout << position.x + chunkLocation.x << " " << position.y + chunkLocation.y << " " << position.z << std::endl;

    // leaves
    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            for (int z = 8; z <= 12; z++) {
                chunkManager.setBlock(
                        {chunkLocation.x + position.x + x, chunkLocation.y + position.y + y,
                         position.z + z},
                        Block(Block::OAK_LEAVES));
            }
        }
    }

}

TerrainGenerator::TerrainGenerator() = default;
