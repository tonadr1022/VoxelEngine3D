//
// Created by Tony Adriansen on 11/16/23.
//

#include "TerrainGenerator.h"
#include "../chunk/Chunk.h"

void TerrainGenerator::generateTerrainFor(Chunk &chunk) {
    glm::ivec2 chunkLocation = chunk.getLocation();

    for (int xPosInChunk = 0; xPosInChunk < CHUNK_WIDTH; xPosInChunk++) {
        for (int yPosInChunk = 0; yPosInChunk < CHUNK_WIDTH; yPosInChunk++) {
            int worldX = chunkLocation.x + xPosInChunk;
            int worldY = chunkLocation.y + yPosInChunk;
            float noiseVal = glm::simplex(glm::vec2(worldX, worldY) / 64.0f);
            noiseVal = (noiseVal + 1.0f) / 2.0f;

            int height = floor(noiseVal * CHUNK_HEIGHT);
            for (int zPosInChunk = 0; zPosInChunk < CHUNK_HEIGHT; zPosInChunk++) {
                if (zPosInChunk < height) {
                    chunk.setBlock(xPosInChunk, yPosInChunk, zPosInChunk, Block(Block::DIRT));
                } else {
                    chunk.setBlock(xPosInChunk, yPosInChunk, zPosInChunk, Block(Block::AIR));
                }
            }
        }
    }
    chunk.chunkState = ChunkState::GENERATED;
}

TerrainGenerator::TerrainGenerator() = default;
