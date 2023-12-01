//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CHUNK_H
#define VOXEL_ENGINE_CHUNK_H

#include "../../Config.h"
#include "ChunkMesh.h"
#include "../chunklet/Chunklet.h"


enum class ChunkMeshState {
    BUILT = 0,
    UNBUILT,
    FAILED,
};

enum class ChunkState {
    GENERATED = 0,
    UNGENERATED,
    CHANGED,
    UNDEFINED,
    FAILED
};

class ChunkManager;

class Chunklet;

class Chunk {
public:
    Chunk() = delete;

    explicit Chunk(glm::vec2 location);

    void buildMesh(Chunk &leftNeighborChunk, Chunk &rightNeighborChunk, Chunk &frontNeighborChunk,
                   Chunk &backNeighborChunk);

    void load();

    void unload();

    void setBlock(int x, int y, int z, Block block);

    Block getBlock(int x, int y, int z);

    ChunkMeshState chunkMeshState = ChunkMeshState::UNBUILT;
    ChunkState chunkState = ChunkState::UNDEFINED;


    glm::vec2 &getLocation();

    ChunkMesh &getMesh();

    std::array<Chunklet, NUM_CHUNKLETS> chunklets;

    int getMaxBlockHeightAt(int x, int y);

    void setMaxBlockHeightAt(int x, int y, int z);

    void markDirty();

    void setIsBlockBuried(int x, int y, int z, bool isBuried);

    std::array<int, CHUNK_HEIGHT> numSolidBlocksInLayers;
private:

    std::array<int, CHUNK_AREA> maxBlockHeights;
    std::array<bool, CHUNK_VOLUME> blocksBuried;
    ChunkMesh mesh;
    glm::vec2 location;
};


#endif //VOXEL_ENGINE_CHUNK_H
