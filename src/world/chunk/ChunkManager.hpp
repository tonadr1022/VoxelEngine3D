//
// Created by Tony Adriansen on 11/22/23.
//

#ifndef VOXEL_ENGINE_CHUNKMANAGER_HPP
#define VOXEL_ENGINE_CHUNKMANAGER_HPP


#include "Chunk.hpp"
#include "ChunkKey.hpp"
#include "../../EngineConfig.hpp"


using ChunkMap = std::unordered_map<ChunkKey, std::shared_ptr<Chunk>>;

class ChunkManager {
public:
    ChunkManager();

    void updateChunkMesh(ChunkKey chunkKey);

    void buildChunkMesh(ChunkKey chunkKey);

    const Ref<Chunk> &getChunk(ChunkKey chunkKey);

    Block getBlock(glm::ivec3 position);

    void setBlock(glm::ivec3 position, Block block);

    bool chunkExists(ChunkKey chunkKey);

    ChunkMap &getChunkMap();

    static ChunkKey getChunkKeyByWorldLocation(int x, int y);

    bool hasAllNeighbors(ChunkKey chunkKey);

    bool hasAllNeighborsFullyGenerated(ChunkKey chunkKey);

    static constexpr std::array<glm::ivec2, 8> NEIGHBOR_CHUNK_KEY_OFFSETS = {
            glm::ivec2{-1, -1},
            glm::ivec2{-1, 0},
            glm::ivec2{-1, 1},
            glm::ivec2{0, -1},
            glm::ivec2{0, 1},
            glm::ivec2{1, -1},
            glm::ivec2{1, 0},
            glm::ivec2{1, 1}
    };

    void handleChunkUpdates(const Ref<Chunk> &chunk, ChunkKey chunkKey, int chunkX, int chunkY);

    void remeshChunksToRemesh();

    void addChunkToRemesh(ChunkKey chunkKey);

private:

    ChunkMap chunkMap;
    std::unordered_set<ChunkKey> m_chunksToRemesh;


};


#endif //VOXEL_ENGINE_CHUNKMANAGER_HPP
