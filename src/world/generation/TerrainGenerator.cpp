//
// Created by Tony Adriansen on 11/16/23.
//

#include "TerrainGenerator.hpp"
#include "../chunk/Chunk.hpp"
#include "../chunk/ChunkManager.hpp"

void TerrainGenerator::generateTerrainFor(const Ref<Chunk> &chunk) {
    glm::ivec2 chunkLocation = chunk->getLocation();

    for (int xPosInChunk = 0; xPosInChunk < CHUNK_WIDTH; xPosInChunk++) {
        for (int yPosInChunk = 0; yPosInChunk < CHUNK_WIDTH; yPosInChunk++) {
            int worldX = chunkLocation.x + xPosInChunk;
            int worldY = chunkLocation.y + yPosInChunk;
            int baseHeight = 32;
            float noiseVal = 0.0f;
            int numOctaves = 4;
            float frequency = 64.0f;
            float amplitude = 32.0f;
            for (int i = 0; i < numOctaves; i++) {
                noiseVal += amplitude * glm::simplex(glm::vec2(worldX, worldY) / frequency);
                frequency *= 2.0f;
                amplitude /= 2.0f;
            }
            noiseVal = (noiseVal + 1.0f) / 2.0f;
            int height = floor(noiseVal + static_cast<float>(baseHeight));

            for (int zPosInChunk = 0; zPosInChunk < CHUNK_HEIGHT; zPosInChunk++) {
                if (zPosInChunk < height) {
                    chunk->setBlock(xPosInChunk, yPosInChunk, zPosInChunk, Block(Block::DIRT));
                } else if (zPosInChunk == height) {
                    chunk->setBlock(xPosInChunk, yPosInChunk, zPosInChunk, Block(Block::GRASS));
                } else {
                    chunk->setBlock(xPosInChunk, yPosInChunk, zPosInChunk, Block(Block::AIR));
                }
            }
        }
    }
    chunk->chunkState = ChunkState::TERRAIN_GENERATED;
}

void TerrainGenerator::generateStructuresFor(ChunkManager &chunkManager, const Ref<Chunk> &chunk) {
    glm::vec2 chunkLocation = chunk->getLocation();
    int height = chunk->getMaxBlockHeightAt(15, 15);

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_WIDTH; y++) {
            for (int z = 0; z < CHUNK_HEIGHT; z++) {
                if (chunk->getBlock(x, y, z).id == Block::GRASS) {
//                    float noiseVal = glm::simplex(
//                            glm::vec2(chunkLocation.x + x, chunkLocation.y + y)/ 64.0f);
//                    noiseVal = (noiseVal + 1.0f) / 2.0f;
                    float r = ((float) rand() / (RAND_MAX));
                    if (r > 0.995f) {
                        makeTree(chunkManager, chunk,
                                 {x, y, z + 1});
                    }
                }
            }
        }
    }
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
                                const glm::vec3 &position) {
    glm::ivec2 chunkLocation = chunk->getLocation();
    for (int i = 0; i < 10; i++) {
        chunkManager.setBlock({chunkLocation.x + position.x, chunkLocation.y + position.y, position.z + i}, Block(Block::OAK_WOOD));
    }

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
