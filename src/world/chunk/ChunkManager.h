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

    void updateChunkMeshes(ChunkKey &playerChunkKeyPos, int renderDistance,
                           bool shouldUpdateAll = false);



    Chunk &getChunk(ChunkKey chunkKey);

    Chunk &getChunkByWorldLocation(int x, int y);

    Block getBlock(glm::ivec3 position);

    void setBlock(glm::ivec3 position, Block block);

    void setBlockAndHandleChunkUpdates(glm::ivec3 position, Block block);

    bool chunkExists(ChunkKey chunkKey);

    ChunkMap &getChunkMap();

    static ChunkKey getChunkKeyByWorldLocation(int x, int y);

    static ChunkKey getNeighborChunkKey(HorizontalDirection direction, ChunkKey &chunkKey);

private:

    std::map<ChunkKey, Chunk> chunkMap;
    std::vector<ChunkKey> chunksToReload;

    void reloadChunksToReload();

    void updateChunkMesh(ChunkKey &chunkKey);

};


#endif //VOXEL_ENGINE_CHUNKMANAGER_H
