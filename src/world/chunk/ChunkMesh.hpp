//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CHUNKMESH_HPP
#define VOXEL_ENGINE_CHUNKMESH_HPP

#include "../block/Block.hpp"
#include <vector>
#include <glm/glm.hpp>
#include <array>

class Chunk;

class ChunkManager;

enum class BlockFace {
    FRONT = 0,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
    COUNT
};

using OcclusionLevels = std::array<uint8_t, 4>;

class ChunkMesh {
public:
    ChunkMesh();

    void construct(ChunkManager &chunkManager, Chunk &chunk, Chunk &leftNeighborChunk,
                   Chunk &rightNeighborChunk,
                   Chunk &frontNeighborChunk,
                   Chunk &backNeighborChunk);

    void clearData();

    void clearBuffers();

    std::vector<uint32_t> vertices;
    std::vector<unsigned int> indices;
    bool isBuffered = false;
    unsigned int VAO, VBO, EBO;


private:
    void addFace(glm::ivec3 &blockPosInChunk, Block &block, BlockFace face, Chunk &chunk,
                 ChunkManager &chunkManager);

    static bool shouldAddFace(glm::ivec3 &blockPosInChunk, Chunk &chunk, Chunk &leftNeighborChunk,
                              Chunk &rightNeighborChunk, Chunk &frontNeighborChunk,
                              Chunk &backNeighborChunk);

    static OcclusionLevels
    getOcclusionLevels(glm::ivec3 &blockPosInChunk, BlockFace face, Chunk &chunk,
                       ChunkManager &chunkManager);

    constexpr static OcclusionLevels NO_OCCLUSION = {3, 3, 3, 3};
};


#endif //VOXEL_ENGINE_CHUNKMESH_HPP
