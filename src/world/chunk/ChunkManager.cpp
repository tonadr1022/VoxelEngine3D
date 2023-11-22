//
// Created by Tony Adriansen on 11/22/23.
//

#include "ChunkManager.h"
#include "../generation/TerrainGenerator.h"

ChunkManager::ChunkManager() {

}

void ChunkManager::updateChunk(Chunk &chunk) {
        TerrainGenerator::generateTerrainFor(chunk);
        chunk.chunkState = ChunkState::GENERATED;
//        chunkMap.emplace(chunkKey, chunk);
}

Chunk &ChunkManager::getChunk(ChunkKey chunkKey) {
    auto it = chunkMap.find(chunkKey);
    if (it == chunkMap.end()) {
        throw std::runtime_error(
                "Chunk not found at: " + std::to_string(chunkKey.x) + ", " +
                std::to_string(chunkKey.y) + "\n");
    }
    return it->second;
}

Chunk &ChunkManager::getChunkByWorldLocation(int x, int y) {
    auto chunkKey = ChunkKey{x / CHUNK_WIDTH, y / CHUNK_WIDTH};
    return getChunk(chunkKey);
}

bool ChunkManager::chunkExists(ChunkKey chunkKey) {
    return chunkMap.find(chunkKey) != chunkMap.end();
}

ChunkMap &ChunkManager::getChunkMap() {
    return chunkMap;
}
