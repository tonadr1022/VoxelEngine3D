//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CHUNK_HPP
#define VOXEL_ENGINE_CHUNK_HPP

#include "ChunkMesh.hpp"
#include "../chunklet/Chunklet.hpp"
#include "ChunkKey.hpp"


enum class ChunkMeshState {
    BUILT = 0,
    UNBUILT,
};

enum class ChunkState {
    TERRAIN_GENERATED = 0,
    FULLY_GENERATED,
    UNGENERATED,
    CHANGED,
    UNDEFINED,
};

class ChunkManager;

class Chunklet;

class Chunk {
public:
    Chunk() = delete;

    ~Chunk();

    explicit Chunk(glm::vec2 location);

    void buildMesh(ChunkManager& chunkManager, Chunk &leftNeighborChunk, Chunk &rightNeighborChunk, Chunk &frontNeighborChunk,
                   Chunk &backNeighborChunk);

    void unload();

    void setBlock(int x, int y, int z, Block block);

    Block getBlock(int x, int y, int z);

    Block getBlock(glm::ivec3& position, ChunkManager &chunkManager);

    bool hasNonAirBlockAt(int x, int y, int z);

    ChunkMeshState chunkMeshState;
    ChunkState chunkState;


    glm::vec2 &getLocation();

    ChunkMesh &getMesh();

    std::array<Chunklet, NUM_CHUNKLETS> chunklets;

    int getMaxBlockHeightAt(int x, int y);

    void setMaxBlockHeightAt(int x, int y, int z);

    void markDirty();

    std::array<int, CHUNK_HEIGHT> numSolidBlocksInLayers{};

    ChunkKey getChunkKey();
private:
    std::array<int, CHUNK_AREA> maxBlockHeights{};
    ChunkMesh mesh;
    glm::vec2 location;
    ChunkKey m_chunkKey;
};


#endif //VOXEL_ENGINE_CHUNK_HPP
