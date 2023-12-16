//
// Created by Tony Adriansen on 11/22/23.
//

#include "ChunkManager.hpp"
#include "../../utils/Utils.hpp"
#include "ChunkRenderer.hpp"

ChunkManager::ChunkManager() = default;

Chunk &ChunkManager::getChunk(ChunkKey chunkKey) {
    auto it = chunkMap.find(chunkKey);
    if (it == chunkMap.end()) {
        throw std::runtime_error(
                "Chunk not found at: " + std::to_string(chunkKey.x) + ", " +
                std::to_string(chunkKey.y) + "\n");
    }
    return it->second;
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

ChunkKey
ChunkManager::calculateNeighborChunkKey(HorizontalDirection direction, ChunkKey &chunkKey) {
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

void ChunkManager::buildChunkMesh(ChunkKey chunkKey) {
    if (!chunkExists(chunkKey)) {
        throw std::runtime_error(
                "Chunk not found at: " + std::to_string(chunkKey.x) + ", " +
                std::to_string(chunkKey.y) + "\n");
    }
    Chunk &chunk = getChunk(chunkKey);

    ChunkKey leftNeighborChunkKey = calculateNeighborChunkKey(
            HorizontalDirection::LEFT, chunkKey);
    ChunkKey rightNeighborChunkKey = calculateNeighborChunkKey(
            HorizontalDirection::RIGHT, chunkKey);
    ChunkKey frontNeighborChunkKey = calculateNeighborChunkKey(
            HorizontalDirection::FRONT, chunkKey);
    ChunkKey backNeighborChunkKey = calculateNeighborChunkKey(
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
    chunk.buildMesh(*this, leftNeighborChunk, rightNeighborChunk, frontNeighborChunk,
                    backNeighborChunk);
}

void ChunkManager::updateChunkMesh(ChunkKey chunkKey) {
    Chunk &chunk = getChunk(chunkKey);
    chunk.unload();
    buildChunkMesh(chunkKey);
}



bool ChunkManager::hasAllNeighbors(ChunkKey &chunkKey) {
    return std::all_of(NEIGHBOR_CHUNK_KEY_OFFSETS.begin(), NEIGHBOR_CHUNK_KEY_OFFSETS.end(),
                       [&](glm::ivec2 offset) {
                           return chunkExists(
                                   ChunkKey{chunkKey.x + offset.x, chunkKey.y + offset.y});
                       });
}
