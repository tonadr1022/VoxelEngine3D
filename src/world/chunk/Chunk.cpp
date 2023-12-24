//
// Created by Tony Adriansen on 11/16/23.
//

#include "Chunk.hpp"
#include "ChunkManager.hpp"
#include "../../utils/Timer.hpp"
#include "ChunkAlg.hpp"

Chunk::Chunk(glm::ivec2 location) : m_location(location), chunkMeshState(ChunkMeshState::UNBUILT),
                                    chunkState(ChunkState::UNGENERATED), m_chunkKey(
                ChunkManager::getChunkKeyByWorldLocation(location.x, location.y)) {
    std::fill_n(m_blocks, CHUNK_VOLUME, Block::AIR);
//    m_maxTerrainHeights.fill(0);
//    numSolidBlocksInLayers.fill(0);
}

Chunk::Chunk(ChunkKey chunkKey) : m_location(chunkKey.x * CHUNK_WIDTH, chunkKey.y * CHUNK_WIDTH),
                                  chunkMeshState(ChunkMeshState::UNBUILT),
                                  chunkState(ChunkState::UNGENERATED), m_chunkKey(chunkKey) {
//    std::fill_n(m_blocks, CHUNK_VOLUME, Block::AIR);
//    m_maxTerrainHeights.fill(0);
//    numSolidBlocksInLayers.fill(0);
}

Chunk::~Chunk() = default;

void Chunk::unload() {
    mesh.clearData();
    mesh.clearBuffers();
    chunkState = ChunkState::UNDEFINED;
    chunkMeshState = ChunkMeshState::UNBUILT;
}


void Chunk::setBlock(int x, int y, int z, Block block) {
    Block oldBlockId = getBlock(x, y, z);
    Block newBlockId = block;
    if (oldBlockId != Block::AIR && block == Block::AIR) {
        numSolidBlocksInLayers[z]--;
    } else if (oldBlockId == Block::AIR && newBlockId != Block::AIR) {
        numSolidBlocksInLayers[z]++;
    }
    m_blocks[XYZ(x, y, z)] = block;
}

glm::ivec2 &Chunk::getLocation() {
    return m_location;
}

ChunkMesh &Chunk::getMesh() {
    return mesh;
}


void Chunk::markDirty() {
    chunkMeshState = ChunkMeshState::UNBUILT;
    chunkState = ChunkState::CHANGED;
}

bool Chunk::hasNonAirBlockAt(int x, int y, int z) const {
    return getBlock(x, y, z) != Block::AIR;
}


Block Chunk::getBlock(glm::ivec3 &position, ChunkManager &chunkManager) {
    if (position.z < 0) return Block::AIR;

    if (position.z >= CHUNK_HEIGHT) {
        return Block::AIR;
    }

    if (position.x < 0 || position.x >= CHUNK_WIDTH || position.y < 0 ||
        position.y >= CHUNK_WIDTH) {
        glm::ivec3 worldLocation = glm::ivec3(m_location, 0) + position;
        Block block = chunkManager.getBlock(worldLocation);

        return block;
    } else {
        Block block = getBlock(position);
        return block;
    }
}

ChunkLoadInfo::ChunkLoadInfo(ChunkKey chunkKey, int seed) : m_chunkKey(chunkKey), m_seed(seed) {

}

void ChunkLoadInfo::process() {
    auto chunkLocation = glm::ivec2(m_chunkKey.x, m_chunkKey.y) * CHUNK_WIDTH;

    FastNoiseSIMD *fastNoise = FastNoiseSIMD::NewFastNoiseSIMD();
    fastNoise->SetSeed(m_seed);
    fastNoise->SetFractalOctaves(4);
    fastNoise->SetFrequency(1.0f / 300.0f);
    float *heightMap = fastNoise->GetSimplexFractalSet(chunkLocation.x, chunkLocation.y, 0,
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

void ChunkLoadInfo::applyTerrain(const Ref<Chunk> &chunk) {
    std::copy(m_blocks, m_blocks + CHUNK_VOLUME, chunk->m_blocks);
    chunk->chunkState = ChunkState::FULLY_GENERATED;
}

ChunkMeshInfo::ChunkMeshInfo(Ref<Chunk> *chunks) : m_chunkKey(chunks[4]->chunkKey()) {
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
    ChunkAlg::constructMesh(m_blocks, m_chunkKey, m_vertices, m_indices);
    m_done = true;
}

void ChunkMeshInfo::applyMesh(const Ref<Chunk> &chunk) {
    chunk->getMesh().vertices = std::move(m_vertices);
    chunk->getMesh().indices = std::move(m_indices);
    chunk->chunkMeshState = ChunkMeshState::BUILT;
}
