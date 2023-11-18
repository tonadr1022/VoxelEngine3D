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

class Chunklet;

class Chunk {
public:
    explicit Chunk(glm::vec2 location);
    void buildMesh();
    void load();
    void unload();
    void setBlock(int x, int y, int z, Block block);
    Block getBlock(int x, int y, int z);
    ChunkMeshState chunkMeshState = ChunkMeshState::UNBUILT;
    ChunkState chunkState = ChunkState::UNDEFINED;
    glm::vec2& getLocation();
    ChunkMesh &getMesh();
//    glm::vec3 getBlockWorldLocation(int x, int y, int z) const;
    std::array<Chunklet, 8> chunklets;

private:
    ChunkMesh mesh;
    glm::vec2 location;
};


#endif //VOXEL_ENGINE_CHUNK_H
