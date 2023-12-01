//
// Created by Tony Adriansen on 11/22/23.
//

#include "ChunkManager.h"
#include "../generation/TerrainGenerator.h"
#include "../../utils/Utils.h"
#include "ChunkRenderer.h"

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


Block ChunkManager::getBlock(glm::ivec3 position) {
    auto chunkKey = getChunkKeyByWorldLocation(position.x, position.y);
    Chunk &chunk = getChunk(chunkKey);
    return chunk.getBlock(Utils::positiveModulo(position.x, CHUNK_WIDTH),
                          Utils::positiveModulo(position.y, CHUNK_WIDTH), position.z);
}

void ChunkManager::setBlock(glm::ivec3 position, Block block) {
    auto chunkKey = getChunkKeyByWorldLocation(position.x, position.y);
    Chunk &chunk = getChunk(chunkKey);
    chunk.setBlock(Utils::positiveModulo(position.x, CHUNK_WIDTH),
                   Utils::positiveModulo(position.y, CHUNK_WIDTH), position.z, block);
}

ChunkKey ChunkManager::getChunkKeyByWorldLocation(int x, int y) {
    return ChunkKey{static_cast<int>(std::floor(static_cast<float>(x) / CHUNK_WIDTH)),
                    static_cast<int>(std::floor(static_cast<float>(y) / CHUNK_WIDTH))};
}

ChunkKey ChunkManager::getNeighborChunkKey(HorizontalDirection direction, ChunkKey &chunkKey) {
    switch (direction) {
        case HorizontalDirection::LEFT:
            return ChunkKey{chunkKey.x, chunkKey.y - 1};
        case HorizontalDirection::RIGHT:
            return ChunkKey{chunkKey.x, chunkKey.y + 1};
        case HorizontalDirection::FRONT:
            return ChunkKey{chunkKey.x + 1, chunkKey.y};
        case HorizontalDirection::BACK:
            return ChunkKey{chunkKey.x - 1, chunkKey.y};
    }
}

void ChunkManager::setBlockAndHandleChunkUpdates(glm::ivec3 position, Block block) {
    auto chunkKey = getChunkKeyByWorldLocation(position.x, position.y);
    Chunk &chunk = getChunk(chunkKey);
    int chunkX = Utils::positiveModulo(position.x, CHUNK_WIDTH);
    int chunkY = Utils::positiveModulo(position.y, CHUNK_WIDTH);

    chunk.setBlock(chunkX, chunkY, position.z, block);

    chunksToReload.push_back(chunkKey);

    chunk.markDirty();

    // need to update adjacent chunks if block is on border
    if (chunkX == 0) {
        ChunkKey backChunkKey = getNeighborChunkKey(HorizontalDirection::BACK, chunkKey);
        Chunk &backChunk = getChunk(backChunkKey);
        chunksToReload.push_back(backChunkKey);
        backChunk.markDirty();
    } else if (chunkX == CHUNK_WIDTH - 1) {
        ChunkKey frontChunkKey = getNeighborChunkKey(HorizontalDirection::FRONT, chunkKey);
        Chunk &frontChunk = getChunk(frontChunkKey);
        chunksToReload.push_back(frontChunkKey);
        frontChunk.markDirty();
    }
    if (chunkY == 0) {
        ChunkKey leftChunkKey = getNeighborChunkKey(HorizontalDirection::LEFT, chunkKey);
        Chunk &leftChunk = getChunk(leftChunkKey);
        chunksToReload.push_back(leftChunkKey);
        leftChunk.markDirty();
    } else if (chunkY == CHUNK_WIDTH - 1) {
        ChunkKey rightChunkKey = getNeighborChunkKey(HorizontalDirection::RIGHT, chunkKey);
        Chunk &rightChunk = getChunk(rightChunkKey);
        chunksToReload.push_back(rightChunkKey);
        rightChunk.markDirty();
    }


}

void ChunkManager::updateChunkMeshes(ChunkKey &playerChunkKeyPos, int renderDistance,  bool shouldUpdateAll) {
    reloadChunksToReload();
    for (int chunkX = playerChunkKeyPos.x - (renderDistance);
         chunkX < playerChunkKeyPos.x + (renderDistance); chunkX++) {
        for (int chunkY = playerChunkKeyPos.y - renderDistance;
             chunkY < playerChunkKeyPos.y + renderDistance; chunkY++) {
            ChunkKey chunkKey = {chunkX, chunkY};
            updateChunkMesh(chunkKey);
            if (!shouldUpdateAll) return;
        }
    }
}

void ChunkManager::reloadChunksToReload() {
    for (auto &chunkKey : chunksToReload) {
        updateChunkMesh(chunkKey);
    }
    chunksToReload.clear();
}

void ChunkManager::updateChunkMesh(ChunkKey &chunkKey) {
    if (!chunkExists(chunkKey)) {
        return;
    }
    Chunk &chunk = getChunk(chunkKey);
    if (chunk.chunkMeshState == ChunkMeshState::BUILT) {
        chunk.unload();
    }
    ChunkKey leftNeighborChunkKey = ChunkManager::getNeighborChunkKey(
            HorizontalDirection::LEFT, chunkKey);
    ChunkKey rightNeighborChunkKey = ChunkManager::getNeighborChunkKey(
            HorizontalDirection::RIGHT, chunkKey);
    ChunkKey frontNeighborChunkKey = ChunkManager::getNeighborChunkKey(
            HorizontalDirection::FRONT, chunkKey);
    ChunkKey backNeighborChunkKey = ChunkManager::getNeighborChunkKey(
            HorizontalDirection::BACK, chunkKey);
    if (!chunkExists(leftNeighborChunkKey) ||
        !chunkExists(rightNeighborChunkKey) ||
        !chunkExists(frontNeighborChunkKey) ||
        !chunkExists(backNeighborChunkKey)) {
        return;
    }
    Chunk leftNeighborChunk = getChunk(leftNeighborChunkKey);
    Chunk rightNeighborChunk = getChunk(rightNeighborChunkKey);
    Chunk frontNeighborChunk = getChunk(frontNeighborChunkKey);
    Chunk backNeighborChunk = getChunk(backNeighborChunkKey);
    chunk.buildMesh(leftNeighborChunk, rightNeighborChunk, frontNeighborChunk,
                    backNeighborChunk);
    ChunkRenderer::createGPUResources(chunk);
}
