//
// Created by Tony Adriansen on 11/16/23.
//

#include "TerrainGenerator.hpp"
#include "../chunk/Chunk.hpp"
#include "../chunk/ChunkManager.hpp"
#include <glm/gtc/noise.hpp>

void TerrainGenerator::generateTerrainFor(Chunk &chunk) {
    glm::ivec2 chunkLocation = chunk.getLocation();

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
                    chunk.setBlock(xPosInChunk, yPosInChunk, zPosInChunk, Block(Block::DIRT));
                } else if (zPosInChunk == height) {
                    chunk.setBlock(xPosInChunk, yPosInChunk, zPosInChunk, Block(Block::GRASS));
                } else {
                    chunk.setBlock(xPosInChunk, yPosInChunk, zPosInChunk, Block(Block::AIR));
                }
            }
        }
    }

    chunk.chunkState = ChunkState::TERRAIN_GENERATED;
}

void TerrainGenerator::generateStructuresFor(ChunkManager &chunkManager, Chunk &chunk) {
    glm::vec2 chunkLocation = chunk.getLocation();
    int height = chunk.getMaxBlockHeightAt(0,0);
    for (int i = 0; i < 10; i++) {
        chunk.setBlock(0,0, height + i, Block(Block::OAK_WOOD));
//chunkManager.setBlock({chunkLocation.x + 15, chunkLocation.y + 14, height + i}, Block(Block::OAK_WOOD));
    }
    // leaves
//    for (int x = 13; x <= 17; x++) {
//        for (int y = 13; y <= 17; y++) {
//            for (int z = height + 10; z <= height + 14; z++) {
//                chunkManager.setBlock({chunkLocation.x + x, chunkLocation.y + y, z}, Block(Block::OAK_LEAVES));
//            }
//        }
//    }
    for (int x = -5; x <= 5; x++) {
        for (int y = -5; y <= 5; y++) {
            for (int z = height + 10; z <= height + 14; z++) {
                chunkManager.setBlock({chunkLocation.x + x, chunkLocation.y + y, z}, Block(Block::OAK_LEAVES));
            }
        }
    }
//    chunkManager.setBlock({chunkLocation.x + 16, chunkLocation.y + 15, height + 10},
//                          Block(Block::OAK_LEAVES));
    chunk.chunkState = ChunkState::FULLY_GENERATED;
}

TerrainGenerator::TerrainGenerator() = default;
