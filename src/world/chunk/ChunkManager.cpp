//
// Created by Tony Adriansen on 11/22/23.
//

#include "ChunkManager.hpp"
#include "../../utils/Utils.hpp"

ChunkManager::ChunkManager() = default;

const Ref<Chunk> &ChunkManager::getChunk(ChunkKey chunkKey) {
    auto it = chunkMap.find(chunkKey);
    if (it == chunkMap.end()) {
        throw std::runtime_error(
                "Chunk not found at: " + std::to_string(chunkKey.x) + ", " +
                std::to_string(chunkKey.y) + "\n");
    }
    return it->second;
}

ChunkMap &ChunkManager::getChunkMap() {
    return chunkMap;
}


Block ChunkManager::getBlock(glm::ivec3 position) {
    auto chunkKey = getChunkKeyByWorldLocation(position.x, position.y);
    const Ref<Chunk> &chunk = getChunk(chunkKey);
    return chunk->getBlock(Utils::positiveModulo(position.x, CHUNK_WIDTH),
                           Utils::positiveModulo(position.y, CHUNK_WIDTH), position.z);
}

void ChunkManager::setBlock(glm::ivec3 position, Block block) {
    auto chunkKey = ChunkManager::getChunkKeyByWorldLocation(position.x, position.y);
    const Ref<Chunk> &chunk = getChunk(chunkKey);
    int chunkX = Utils::positiveModulo(position.x, CHUNK_WIDTH);
    int chunkY = Utils::positiveModulo(position.y, CHUNK_WIDTH);

    chunk->setBlock(chunkX, chunkY, position.z, block);
    handleChunkUpdates(chunk, chunkKey, chunkX, chunkY);
}

ChunkKey ChunkManager::getChunkKeyByWorldLocation(int x, int y) {
    return ChunkKey{static_cast<int>(std::floor(static_cast<float>(x) / CHUNK_WIDTH)),
                    static_cast<int>(std::floor(static_cast<float>(y) / CHUNK_WIDTH))};
}

void ChunkManager::buildChunkMesh(ChunkKey chunkKey) {
    if (!chunkExists(chunkKey)) {
        throw std::runtime_error(
                "Cannot mesh, chunk not found at: " + std::to_string(chunkKey.x) + ", " +
                std::to_string(chunkKey.y) + "\n");
    }
    const Ref<Chunk> &chunk = getChunk(chunkKey);
    chunk->getMesh().construct(*this, chunk);
    chunk->chunkMeshState = ChunkMeshState::BUILT;
}

void ChunkManager::remeshChunk(ChunkKey chunkKey) {
    const Ref<Chunk> &chunk = getChunk(chunkKey);
    ChunkMesh &mesh = chunk->getMesh();
    mesh.clearData();
    mesh.clearBuffers();
    chunk->chunkMeshState = ChunkMeshState::UNBUILT;
    mesh.construct(*this, chunk);
    chunk->chunkMeshState = ChunkMeshState::BUILT;
}


bool ChunkManager::hasAllNeighbors(ChunkKey chunkKey) {
    return std::all_of(NEIGHBOR_CHUNK_KEY_OFFSETS.begin(), NEIGHBOR_CHUNK_KEY_OFFSETS.end(),
                       [&](glm::ivec2 offset) {
                           return chunkExists(
                                   ChunkKey{chunkKey.x + offset.x, chunkKey.y + offset.y});
                       });
}

bool ChunkManager::hasAllNeighborsFullyGenerated(ChunkKey chunkKey) {
    return std::all_of(NEIGHBOR_CHUNK_KEY_OFFSETS.begin(), NEIGHBOR_CHUNK_KEY_OFFSETS.end(),
                       [&](glm::ivec2 offset) {
                           auto neighborChunkKey = ChunkKey{chunkKey.x + offset.x,
                                                            chunkKey.y + offset.y};
                           return chunkExists(neighborChunkKey) &&
                                  getChunk(neighborChunkKey)->chunkState ==
                                  ChunkState::FULLY_GENERATED;
                       });
}


void ChunkManager::addChunkToRemesh(ChunkKey chunkKey) {
    m_chunksToRemesh.insert(chunkKey);
}

void ChunkManager::handleChunkUpdates(const Ref<Chunk> &chunk, ChunkKey chunkKey, int chunkX,
                                      int chunkY) {
    // don't use lambda since we already have a reference to the chunk
    if (chunk->chunkMeshState == ChunkMeshState::BUILT) {
        std::cout << "adding chunk to remesh (itself): " << chunkKey.x << ", " << chunkKey.y
                  << std::endl;
        addChunkToRemesh(chunkKey);
        chunk->markDirty();
    }

    // lambda to handle neighbor chunk updates
    auto handleNeighborChunk = [&](int offsetX, int offsetY) {
        auto neighborChunkKey = ChunkKey{chunkKey.x + offsetX, chunkKey.y + offsetY};
        const Ref<Chunk> &neighborChunk = getChunk(neighborChunkKey);
        if (neighborChunk->chunkMeshState == ChunkMeshState::BUILT) {
            std::cout << "adding chunk to remesh: " << neighborChunkKey.x << ", "
                      << neighborChunkKey.y << ". offset: " << offsetX << ", " << offsetY
                      << ". original: " << chunkKey.x << ", " << chunkKey.y << std::endl;
            addChunkToRemesh(neighborChunkKey);
            neighborChunk->markDirty();
        }
    };

    // X axis side chunks
    if (chunkX == 0) {
        handleNeighborChunk(-1, 0);
    } else if (chunkX == CHUNK_WIDTH - 1) {
        handleNeighborChunk(1, 0);
    }

    // Y axis side chunks
    if (chunkY == 0) {
        handleNeighborChunk(0, -1);
    } else if (chunkY == CHUNK_WIDTH - 1) {
        handleNeighborChunk(0, 1);
    }

    // corner chunks
    if (chunkX == 0 && chunkY == 0) {
        handleNeighborChunk(-1, -1);
    } else if (chunkX == 0 && chunkY == CHUNK_WIDTH - 1) {
        handleNeighborChunk(-1, 1);
    } else if (chunkX == CHUNK_WIDTH - 1 && chunkY == 0) {
        handleNeighborChunk(1, -1);
    } else if (chunkX == CHUNK_WIDTH - 1 && chunkY == CHUNK_WIDTH - 1) {
        handleNeighborChunk(1, 1);
    }
}

void ChunkManager::remeshChunksToRemesh() {
    if (!m_chunksToRemesh.empty()) {
        std::cout << "remeshing " << m_chunksToRemesh.size() << " chunks" << std::endl;
    }
    for (auto &chunkKey: m_chunksToRemesh) {
        remeshChunk(chunkKey);
    }
    m_chunksToRemesh.clear();
}
