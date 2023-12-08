//
// Created by Tony Adriansen on 11/22/23.
//

#ifndef VOXEL_ENGINE_CHUNKMANAGER_H
#define VOXEL_ENGINE_CHUNKMANAGER_H


#include "Chunk.h"
#include "ChunkKey.h"
#include <map>

using ChunkMap = std::map<ChunkKey, Chunk>;

class ChunkManager {
public:
    ChunkManager();

    void updateChunk(Chunk &chunk);

    void updateChunkMesh(ChunkKey &chunkKey);

    void buildChunkMesh(ChunkKey &chunkKey);

    Chunk &getChunk(ChunkKey chunkKey);

    Chunk &getChunkByWorldLocation(int x, int y);

    Block getBlock(glm::ivec3 position);

    void setBlock(glm::ivec3 position, Block block);

    bool chunkExists(ChunkKey chunkKey);

    ChunkMap &getChunkMap();

    static ChunkKey getChunkKeyByWorldLocation(int x, int y);

    static ChunkKey calculateNeighborChunkKey(HorizontalDirection direction, ChunkKey &chunkKey);

private:

    ChunkMap chunkMap;



};


#endif //VOXEL_ENGINE_CHUNKMANAGER_H
