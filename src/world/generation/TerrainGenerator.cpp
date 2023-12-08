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
            float noiseVal = glm::simplex(glm::vec2(worldX, worldY) / 64.0f);
            noiseVal = (noiseVal + 1.0f) / 2.0f;
            int height = floor(noiseVal * 64.0f);
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
//    std::cout << chunk.numTransparentBlocksInLayers[10] << std::endl;
//    auto start = std::chrono::high_resolution_clock::now();
//    setBuriedBlockStates(chunk);
//    auto end = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//
//        std::cout << "init transparency array: " << duration.count() << "microseconds"
//                  << std::endl;

    chunk.chunkState = ChunkState::GENERATED;
}


void TerrainGenerator::setBuriedBlockStates(Chunk &chunk) {
    glm::ivec2 chunkLocation = chunk.getLocation();
    for (int xPosInChunk = 0; xPosInChunk < CHUNK_WIDTH; xPosInChunk++) {
        for (int yPosInChunk = 0; yPosInChunk < CHUNK_WIDTH; yPosInChunk++) {
            for (int zPosInChunk = 0; zPosInChunk < CHUNK_HEIGHT; zPosInChunk++) {
                int maxBlockHeight = chunk.getMaxBlockHeightAt(xPosInChunk, yPosInChunk);
                if (zPosInChunk > maxBlockHeight) {
                    break;
                }
                Block block = chunk.getBlock(xPosInChunk, yPosInChunk, zPosInChunk);
//                 if air skip
                if (block.id == Block::AIR) {
                    continue;
                }
                // check neighbors, if all are not transparent, set to buried
                // check if out of chunk bounds

                bool isBuried = true;

                // below neighbor
                Block belowNeighbor = chunk.getBlock(xPosInChunk, yPosInChunk, zPosInChunk - 1);
                if (belowNeighbor.id == Block::AIR || belowNeighbor.id == Block::UNDEFINED) {
                    isBuried = false;
                }

                // above neighbor
                Block aboveNeighbor = chunk.getBlock(xPosInChunk, yPosInChunk, zPosInChunk + 1);
                if (aboveNeighbor.id == Block::AIR || aboveNeighbor.id == Block::UNDEFINED) {
                    isBuried = false;
                }

                // left neighbor
                Block leftNeighbor = chunk.getBlock(xPosInChunk, yPosInChunk - 1, zPosInChunk);
                if (leftNeighbor.id == Block::AIR || leftNeighbor.id == Block::UNDEFINED) {
                    isBuried = false;
                }

                // right neighbor
                Block rightNeighbor = chunk.getBlock(xPosInChunk, yPosInChunk + 1, zPosInChunk);
                if (rightNeighbor.id == Block::AIR || rightNeighbor.id == Block::UNDEFINED) {
                    isBuried = false;
                }


                // front neighbor
                Block frontNeighbor = chunk.getBlock(xPosInChunk + 1, yPosInChunk, zPosInChunk);
                if (frontNeighbor.id == Block::AIR || frontNeighbor.id == Block::UNDEFINED) {
                    isBuried = false;
                }

                // back neighbor
                Block backNeighbor = chunk.getBlock(xPosInChunk - 1, yPosInChunk, zPosInChunk);
                if (backNeighbor.id == Block::AIR || backNeighbor.id == Block::UNDEFINED) {
                    isBuried = false;
                }

                if (isBuried) {
                    chunk.setIsBlockBuried(xPosInChunk, yPosInChunk, zPosInChunk, true);
                } else {
                    chunk.setIsBlockBuried(xPosInChunk, yPosInChunk, zPosInChunk, false);
                }
            }
        }
    }
}

TerrainGenerator::TerrainGenerator() = default;
