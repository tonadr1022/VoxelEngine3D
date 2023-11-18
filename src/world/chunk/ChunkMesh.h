//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CHUNKMESH_H
#define VOXEL_ENGINE_CHUNKMESH_H

#include "../../Config.h"
#include "../block/Block.h"

class Chunk;

enum class BlockFace {
    FRONT = 0,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
    COUNT
};

class ChunkMesh {
public:
    ChunkMesh();

    void construct(Chunk &chunk);

    void destruct();

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;

private:
    void addFace(glm::ivec3& blockPosInChunk, Block block, BlockFace face);

    static bool shouldAddFace(glm::ivec3 &blockPosInChunk, Block block, BlockFace face);

};


#endif //VOXEL_ENGINE_CHUNKMESH_H
