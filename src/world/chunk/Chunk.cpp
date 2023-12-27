//
// Created by Tony Adriansen on 11/16/23.
//

#include "Chunk.hpp"
#include "ChunkManager.hpp"
#include "../../utils/Timer.hpp"
#include "ChunkAlg.hpp"

Chunk::Chunk(glm::ivec2 pos)
        : m_pos(pos), m_worldPos(pos * CHUNK_WIDTH),
          chunkMeshState(ChunkMeshState::UNBUILT),
          chunkState(ChunkState::UNGENERATED) {
}

Chunk::~Chunk() = default;

void Chunk::unload() {
    mesh.clearData();
    mesh.clearBuffers();
    chunkState = ChunkState::UNDEFINED;
    chunkMeshState = ChunkMeshState::UNBUILT;
}


void Chunk::setBlock(int x, int y, int z, Block block) {
    m_blocks[XYZ(x, y, z)] = block;
}

ChunkMesh &Chunk::getMesh() {
    return mesh;
}


void Chunk::markDirty() {
    chunkMeshState = ChunkMeshState::UNBUILT;
    chunkState = ChunkState::CHANGED;
}

ChunkLoadInfo::ChunkLoadInfo(glm::ivec2 pos, int seed) : m_pos(pos), m_seed(seed) {
}

void ChunkLoadInfo::process() {
    auto chunkWorldPos = glm::ivec2(m_pos.x * CHUNK_WIDTH, m_pos.y * CHUNK_WIDTH);

    FastNoiseSIMD *fastNoise = FastNoiseSIMD::NewFastNoiseSIMD();
    fastNoise->SetSeed(m_seed);
    fastNoise->SetFractalOctaves(4);
    fastNoise->SetFrequency(1.0f / 300.0f);
    float *heightMap = fastNoise->GetSimplexFractalSet(chunkWorldPos.x, chunkWorldPos.y, 0,
                                                       CHUNK_WIDTH, CHUNK_WIDTH, 1);

    int heights[CHUNK_AREA];
    int highest = 0;

    for (int i = 0; i < CHUNK_AREA; i++) {
        heights[i] = (int) floor(heightMap[i] * 64) + 100;
        highest = std::max(highest, heights[i]);
    }

    for (int z = 0; z <= highest; z++) {
        int heightMapIndex = 0;
        for (int x = 0; x < CHUNK_WIDTH; x++) {
            for (int y = 0; y < CHUNK_WIDTH; y++) {
                int height = heights[heightMapIndex];
                if (z < height) {
                    m_blocks[XYZ(x, y, z)] = Block::DIRT;
                } else if (z == height) {
                    m_blocks[XYZ(x, y, z)] = Block::GRASS;
                }
                heightMapIndex++;
            }
        }
    }
    m_done = true;
}

void ChunkLoadInfo::applyTerrain(Chunk *chunk) {
    std::copy(m_blocks, m_blocks + CHUNK_VOLUME, chunk->m_blocks);
    chunk->chunkState = ChunkState::FULLY_GENERATED;
}

ChunkMeshInfo::ChunkMeshInfo(Chunk *(&chunks)[9]) : m_pos(chunks[4]->m_pos) {
    // copy the edges of neighboring chunks into array
    // we know all exist

    /*
     * Neighbor Chunks Array Structure
     *
     * \------------------ x
     *  \  0  3  6
     *   \  1  4  7
     *    \  2  5  8
     *     y
     */
    int copyFromBlockIndex, newMeshIndex, chunkIndex, x, y, z;
#define SET m_blocks[newMeshIndex] = chunks[chunkIndex]->m_blocks[copyFromBlockIndex];

// as arg to MESH_XYZ, CHUNK_WIDTH corresponds to CHUNK_WIDTH - 1 + 1
    for (z = 0; z < CHUNK_HEIGHT; z++) {
        // Chunk 0 {x: -1, y: -1}: need the column x=15, y=15, 0 <= z <= 256
        chunkIndex = 0;
        copyFromBlockIndex = XYZ(CHUNK_WIDTH - 1, CHUNK_WIDTH - 1, z);
        newMeshIndex = MESH_XYZ(-1, -1, z);
        SET;

        // Chunk 2 {x: -1, y: 1}: need column x=15, y=0, 0 <= z <= 256
        chunkIndex = 2;
        copyFromBlockIndex = XYZ(CHUNK_WIDTH - 1, 0, z);
        newMeshIndex = MESH_XYZ(-1, CHUNK_WIDTH, z);
        SET;

        // Chunk 6 {x: 1, y: -1}: need column x=0, y=15, 0 <= z <= 256
        chunkIndex = 6;
        copyFromBlockIndex = XYZ(0, CHUNK_WIDTH - 1, z);
        newMeshIndex = MESH_XYZ(CHUNK_WIDTH, -1, z);
        SET;

        // Chunk 8 {x: 1, y: 1}: need column x=0, y=0, 0 <= z <= 256
        chunkIndex = 8;
        copyFromBlockIndex = XYZ(0, 0, z);
        newMeshIndex = MESH_XYZ(CHUNK_WIDTH, CHUNK_WIDTH, z);
        SET;

        for (x = 0; x < CHUNK_WIDTH; x++) {
            // Chunk 3 {x: 0, y: -1}: need rectangle 0 <= x <= 15, y = 15, 0 <= z <= 256
            chunkIndex = 3;
            copyFromBlockIndex = XYZ(x, CHUNK_WIDTH - 1, z);
            newMeshIndex = MESH_XYZ(x, -1, z);
            SET;

            // Chunk 5 {x: 0, y: 1}: need rectangle 0 <= x <= 15, y = 0, 0 <= z <= 256
            chunkIndex = 5;
            copyFromBlockIndex = XYZ(x, 0, z);
            newMeshIndex = MESH_XYZ(x, CHUNK_WIDTH, z);
            SET;

            for (y = 0; y < CHUNK_WIDTH; y++) {
                // Chunk 4 {x: 0, y: 0}: 0 <= x <= 15, 0 <= y <= 15, 0 <= z <= 256
                chunkIndex = 4;
                copyFromBlockIndex = XYZ(x, y, z);
                newMeshIndex = MESH_XYZ(x, y, z);
                SET;
            }
        }

        for (y = 0; y < CHUNK_WIDTH; y++) {
            // Chunk 1 {x: -1, y: 0}: need the rectangle 0 <= y <= 15, x = 15, 0 <= z <= 256
            chunkIndex = 1;
            copyFromBlockIndex = XYZ(CHUNK_WIDTH - 1, y, z);
            newMeshIndex = MESH_XYZ(-1, y, z);
            SET;

            // Chunk 7 {x: 1, y: 0}: need rectangle 0 <= y <= 15, x = 0, 0 <= z <= 256
            chunkIndex = 7;
            copyFromBlockIndex = XYZ(0, y, z);
            newMeshIndex = MESH_XYZ(CHUNK_WIDTH, y, z);
            SET;
        }
    }
}

void ChunkMeshInfo::process() {
    ChunkAlg::constructMesh(m_blocks, m_vertices, m_indices);
    m_done = true;
}

void ChunkMeshInfo::applyMesh(Chunk *chunk) {
    chunk->getMesh().vertices = std::move(m_vertices);
    chunk->getMesh().indices = std::move(m_indices);
    chunk->chunkMeshState = ChunkMeshState::BUILT;
}
