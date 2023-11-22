//
// Created by Tony Adriansen on 11/22/23.
//

#ifndef VOXEL_ENGINE_CHUNKMANAGER_H
#define VOXEL_ENGINE_CHUNKMANAGER_H


#include "Chunk.h"
#include "ChunkKey.h"

using ChunkMap = std::map<ChunkKey, Chunk>;

class ChunkManager {
public:
    ChunkManager();

    void updateChunk(Chunk &chunk);

    void unload(Chunk &chunk);

    Chunk &getChunk(ChunkKey chunkKey);

    Chunk &getChunkByWorldLocation(int x, int y);

    bool chunkExists(ChunkKey chunkKey);

    ChunkMap &getChunkMap();



private:
    std::map<ChunkKey, Chunk> chunkMap;

};


#endif //VOXEL_ENGINE_CHUNKMANAGER_H
