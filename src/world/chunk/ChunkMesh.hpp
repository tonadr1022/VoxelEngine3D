//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CHUNKMESH_HPP
#define VOXEL_ENGINE_CHUNKMESH_HPP

#include "../block/Block.hpp"
#include "../../EngineConfig.hpp"

class Chunk;

class ChunkManager;

enum BlockFace : uint8_t {
    FRONT = 0,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};

using OcclusionLevels = std::array<uint8_t, 4>;

class ChunkMesh {
public:
    ChunkMesh();
    ~ChunkMesh() = default;

    void construct(ChunkManager &chunkManager, const Ref<Chunk> &chunk);

    void clearData();

    void clearBuffers();

    std::vector<uint32_t> vertices;
    std::vector<unsigned int> indices;
    bool isBuffered = false;
    unsigned int VAO, VBO, EBO;


private:
//    void addFace(glm::ivec3 &blockPosInChunk, Block block, BlockFace face, const Ref<Chunk> &chunk,
//                 ChunkManager &chunkManager);

    static bool shouldAddFace(glm::ivec3 &blockPosInChunk, const Ref<Chunk> &chunk,
                              const Ref<Chunk> &leftNeighborChunk,
                              const Ref<Chunk> &rightNeighborChunk,
                              const Ref<Chunk> &frontNeighborChunk,
                              const Ref<Chunk> &backNeighborChunk);

//    static OcclusionLevels
//    getOcclusionLevels(glm::ivec3 &blockPosInChunk, BlockFace face, const Ref<Chunk> &chunk,
//                       ChunkManager &chunkManager);

    constexpr static OcclusionLevels NO_OCCLUSION = {3, 3, 3, 3};
};


#endif //VOXEL_ENGINE_CHUNKMESH_HPP
