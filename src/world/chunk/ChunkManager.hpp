//
// Created by Tony Adriansen on 11/22/23.
//

#ifndef VOXEL_ENGINE_CHUNKMANAGER_HPP
#define VOXEL_ENGINE_CHUNKMANAGER_HPP


#include "Chunk.hpp"
#include "ChunkKey.hpp"
#include <map>

using ChunkMap = std::unordered_map<ChunkKey, Chunk>;

class ChunkManager {
public:
    ChunkManager();

    void updateChunkMesh(ChunkKey chunkKey);

    void buildChunkMesh(ChunkKey chunkKey);

    Chunk &getChunk(ChunkKey chunkKey);

    Block getBlock(glm::ivec3 position);

    void setBlock(glm::ivec3 position, Block block);

    bool chunkExists(ChunkKey chunkKey);

    ChunkMap &getChunkMap();

    static ChunkKey getChunkKeyByWorldLocation(int x, int y);

    static ChunkKey calculateNeighborChunkKey(HorizontalDirection direction, ChunkKey &chunkKey);

    bool hasAllNeighbors(ChunkKey &chunkKey);

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

private:

    ChunkMap chunkMap;



};


#endif //VOXEL_ENGINE_CHUNKMANAGER_HPP
