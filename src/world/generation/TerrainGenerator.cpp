//
// Created by Tony Adriansen on 11/16/23.
//

#include "TerrainGenerator.h"
#include "../chunk/Chunk.h"
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
            int height = floor(noiseVal + baseHeight);

            chunk.setMaxBlockHeightAt(xPosInChunk, yPosInChunk, height);

            for (int zPosInChunk = 0; zPosInChunk < CHUNK_HEIGHT; zPosInChunk++) {
                if (zPosInChunk < height) {
                    chunk.setBlock(xPosInChunk, yPosInChunk, zPosInChunk, Block(Block::DIRT));
                } else if (zPosInChunk == height) {
                    chunk.setBlock(xPosInChunk, yPosInChunk, zPosInChunk, Block(Block::GRASS));
                } else {
                    chunk.setBlock(xPosInChunk, yPosInChunk, zPosInChunk, Block(Block::AIR));
                }
            }

            if (xPosInChunk == 8 && yPosInChunk == 8) {
                for (int i = 0; i < 10; i++) {
                    chunk.setBlock(xPosInChunk, yPosInChunk, i + height, Block(Block::OAK_WOOD));
                }
                chunk.setMaxBlockHeightAt(xPosInChunk, yPosInChunk, height + 10);
            }
        }
    }

    chunk.chunkState = ChunkState::GENERATED;
}

TerrainGenerator::TerrainGenerator() = default;
