//
// Created by Tony Adriansen on 11/16/23.
//

#include "ChunkMesh.hpp"
#include "Chunk.hpp"
#include "../block/BlockDB.hpp"
#include "ChunkManager.hpp"


namespace {
    constexpr std::array<int, 20> frontFace{
            1, 0, 0, 0, 0, // bottom left
            1, 1, 0, 1, 0, // bottom right
            1, 0, 1, 0, 1, // top left
            1, 1, 1, 1, 1, // top right
    };

    constexpr std::array<int, 20> backFace{
            0, 0, 0, 0, 0,
            0, 0, 1, 0, 1,
            0, 1, 0, 1, 0,
            0, 1, 1, 1, 1,
    };

    constexpr std::array<int, 20> leftFace{
            0, 0, 0, 0, 0,
            1, 0, 0, 1, 0,
            0, 0, 1, 0, 1,
            1, 0, 1, 1, 1,
    };

    constexpr std::array<int, 20> rightFace{
            0, 1, 0, 0, 0,
            0, 1, 1, 0, 1,
            1, 1, 0, 1, 0,
            1, 1, 1, 1, 1,
    };

    constexpr std::array<int, 20> topFace{
            0, 0, 1, 0, 0,
            1, 0, 1, 1, 0,
            0, 1, 1, 0, 1,
            1, 1, 1, 1, 1,
    };

    constexpr std::array<int, 20> bottomFace{
            0, 0, 0, 0, 0,
            0, 1, 0, 0, 1,
            1, 0, 0, 1, 0,
            1, 1, 0, 1, 1,
    };

    constexpr std::array<std::array<std::array<glm::ivec3, 3>, 4>, 6> lightingAdjacencies =
            {{
                     // front face
                     {{
                              // Bottom Left
                              {glm::ivec3(1, 0, -1), glm::ivec3(1, -1, 0), glm::ivec3(1, -1, -1)},
                              // Bottom Right
                              {glm::ivec3(1, 0, -1), glm::ivec3(1, 1, 0), glm::ivec3(1, 1, -1)},
                              // Top Left
                              {glm::ivec3(1, 0, 1), glm::ivec3(1, -1, 0), glm::ivec3(1, -1, 1)},
                              // Top Right
                              {glm::ivec3(1, 0, 1), glm::ivec3(1, 1, 0), glm::ivec3(1, 1, 1)},
                      }},
                     // back face
                     {{
                              // 0, 0, 0
                              {glm::ivec3(-1, 0, -1), glm::ivec3(-1, -1, 0),
                               glm::ivec3(-1, -1, -1)},
                              // 0, 0, 1
                              {glm::ivec3(-1, 0, 1), glm::ivec3(-1, -1, 0), glm::ivec3(-1, -1, 1)},
                              // 0, 1, 0
                              {glm::ivec3(-1, 0, -1), glm::ivec3(-1, 1, 0), glm::ivec3(-1, 1, -1)},
                              // 0, 1, 1
                              {glm::ivec3(-1, 1, 0), glm::ivec3(-1, 0, 1), glm::ivec3(-1, 1, 1)},
                      }},
                     // left face
                     {{
                              // 0, 0, 0
                              {glm::ivec3(0, -1, -1), glm::ivec3(-1, -1, 0),
                               glm::ivec3(-1, -1, -1)},
                              // 1, 0, 0
                              {glm::ivec3(0, -1, -1), glm::ivec3(1, -1, 0), glm::ivec3(1, -1, -1)},
                              // 0, 0, 1
                              {glm::ivec3(-1, -1, 0), glm::ivec3(0, -1, 1), glm::ivec3(-1, -1, 1)},
                              // 1, 0, 1
                              {glm::ivec3(0, -1, 1), glm::ivec3(1, -1, 0), glm::ivec3(1, -1, 1)},
                      }},
                     // right face
                     {{
                              // 0, 1, 0
                              {glm::ivec3(0, 1, -1), glm::ivec3(-1, 1, 0), glm::ivec3(-1, 1, -1)},
                              // 0, 1, 1
                              {glm::ivec3(0, 1, 1), glm::ivec3(-1, 1, 0), glm::ivec3(-1, 1, 1)},
                              // 1, 1, 0
                              {glm::ivec3(0, 1, -1), glm::ivec3(1, 1, 0), glm::ivec3(1, 1, -1)},
                              // 1, 1, 1
                              {glm::ivec3(0, 1, 1), glm::ivec3(1, 1, 0), glm::ivec3(1, 1, 1)},
                      }},
                     // top face
                     {{
                              // 0, 0, 1
                              {glm::ivec3(0, -1, 1), glm::ivec3(-1, 0, 1), glm::ivec3(-1, -1, 1)},
                              // 1, 0, 1
                              {glm::ivec3(0, -1, 1), glm::ivec3(1, 0, 1), glm::ivec3(1, -1, 1)},
                              // 0, 1, 1
                              {glm::ivec3(0, 1, 1), glm::ivec3(-1, 0, 1), glm::ivec3(-1, 1, 1)},
                              // 1, 1, 1
                              {glm::ivec3(0, 1, 1), glm::ivec3(1, 0, 1), glm::ivec3(1, 1, 1)},
                      }},

                     // bottom face
                     {{
                              // 0, 0, 0
                              {glm::ivec3(0, -1, -1), glm::ivec3(-1, 0, -1),
                               glm::ivec3(-1, -1, -1)},
                              // 0, 1, 0
                              {glm::ivec3(0, 1, -1), glm::ivec3(-1, 0, -1), glm::ivec3(-1, 1, -1)},
                              // 1, 0, 0
                              {glm::ivec3(0, -1, -1), glm::ivec3(1, 0, -1), glm::ivec3(1, -1, -1)},
                              // 1, 1, 0
                              {glm::ivec3(0, 1, -1), glm::ivec3(1, 0, -1), glm::ivec3(1, 1, -1)},
                      }},
             }};
} // namespace


struct AdjacentBlockPositions {
    glm::ivec3 positions[6];

    void update(int x, int y, int z) {
        positions[0] = {x + 1, y, z};
        positions[1] = {x - 1, y, z};
        positions[2] = {x, y - 1, z};
        positions[3] = {x, y + 1, z};
        positions[4] = {x, y, z + 1};
        positions[5] = {x, y, z - 1};
    }
};

ChunkMesh::ChunkMesh() : VAO(0), VBO(0), EBO(0) {
}

bool
ChunkMesh::shouldAddFace(glm::ivec3 &adjacentBlockPosInChunk, const Ref<Chunk> &chunk,
                         const Ref<Chunk> &leftNeighborChunk, const Ref<Chunk> &rightNeighborChunk,
                         const Ref<Chunk> &frontNeighborChunk,
                         const Ref<Chunk> &backNeighborChunk) {
    // check if below min height, if so don't add face
    if (adjacentBlockPosInChunk.z < 0) return false;

    // check if adjacent block is vertically out of bounds, if so add face
    if (adjacentBlockPosInChunk.z > CHUNK_HEIGHT) return true;

    if (adjacentBlockPosInChunk.x < 0) {
        return !backNeighborChunk->hasNonAirBlockAt(CHUNK_WIDTH - 1, adjacentBlockPosInChunk.y,
                                                    adjacentBlockPosInChunk.z);
    } else if (adjacentBlockPosInChunk.x >= CHUNK_WIDTH) {
        return !frontNeighborChunk->hasNonAirBlockAt(0, adjacentBlockPosInChunk.y,
                                                     adjacentBlockPosInChunk.z);
    } else if (adjacentBlockPosInChunk.y < 0) {
        return !leftNeighborChunk->hasNonAirBlockAt(adjacentBlockPosInChunk.x, CHUNK_WIDTH - 1,
                                                    adjacentBlockPosInChunk.z);
    } else if (adjacentBlockPosInChunk.y >= CHUNK_WIDTH) {
        return !rightNeighborChunk->hasNonAirBlockAt(adjacentBlockPosInChunk.x, 0,
                                                     adjacentBlockPosInChunk.z);

    } else {
        return BlockMethods::isTransparent(chunk->getBlock(adjacentBlockPosInChunk.x, adjacentBlockPosInChunk.y,
                               adjacentBlockPosInChunk.z));
    }
}

void ChunkMesh::construct(ChunkManager &chunkManager, const Ref<Chunk> &chunk) {
    clearData();
    AdjacentBlockPositions adjacentBlockPositions{};
    ChunkKey chunkKey = chunk->getChunkKey();
    const Ref<Chunk> &leftNeighborChunk = chunkManager.getChunk({chunkKey.x, chunkKey.y - 1});
    const Ref<Chunk> &rightNeighborChunk = chunkManager.getChunk({chunkKey.x, chunkKey.y + 1});
    const Ref<Chunk> &frontNeighborChunk = chunkManager.getChunk({chunkKey.x + 1, chunkKey.y});
    const Ref<Chunk> &backNeighborChunk = chunkManager.getChunk({chunkKey.x - 1, chunkKey.y});

    for (int z = 0; z < CHUNK_HEIGHT; z++) {
        for (int x = 0; x < CHUNK_WIDTH; x++) {
            for (int y = 0; y < CHUNK_WIDTH; y++) {
                Block block = chunk->getBlock(x, y, z);
                if (block == Block::AIR) {
                    continue;
                }
                glm::ivec3 pos = {x, y, z};
                adjacentBlockPositions.update(x, y, z);
                for (int faceIndex = 0; faceIndex < 6; faceIndex++) {
                    auto face = static_cast<BlockFace>(faceIndex);
                    glm::ivec3 adjacentBlockPos = adjacentBlockPositions.positions[static_cast<int>(face)];
                    if (shouldAddFace(adjacentBlockPos, chunk, leftNeighborChunk,
                                      rightNeighborChunk, frontNeighborChunk,
                                      backNeighborChunk)) {
                        // calculate ambient occlusion level for each vertex of this face
                        addFace(pos, block, face, chunk, chunkManager);
                    }
                }
            }
        }
    }
}

void ChunkMesh::addFace(glm::ivec3 &blockPosInChunk, Block block, BlockFace face,
                        const Ref<Chunk> &chunk,
                        ChunkManager &chunkManager) {
    BlockData &blockData = BlockDB::getBlockData(block);
    int textureX = 0;
    int textureY = 0;
    std::array<int, 20> faceVertices{};

    OcclusionLevels occlusionLevels = getOcclusionLevels(blockPosInChunk, face, chunk,
                                                         chunkManager);

    switch (face) {
        case BlockFace::FRONT:
            faceVertices = frontFace;
            textureX = blockData.frontTexCoords.x;
            textureY = blockData.frontTexCoords.y;
            break;
        case BlockFace::BACK:
            faceVertices = backFace;
            textureX = blockData.backTexCoords.x;
            textureY = blockData.backTexCoords.y;
            break;
        case BlockFace::LEFT:
            faceVertices = leftFace;
            textureX = blockData.leftTexCoords.x;
            textureY = blockData.leftTexCoords.y;
            break;
        case BlockFace::RIGHT:
            faceVertices = rightFace;
            textureX = blockData.rightTexCoords.x;
            textureY = blockData.rightTexCoords.y;
            break;
        case BlockFace::TOP:
            faceVertices = topFace;
            textureX = blockData.topTexCoords.x;
            textureY = blockData.topTexCoords.y;
            break;
        case BlockFace::BOTTOM:
            faceVertices = bottomFace;
            textureX = blockData.bottomTexCoords.x;
            textureY = blockData.bottomTexCoords.y;
            break;
        default:
            break;
    }
    auto baseVertexIndex = vertices.size();
    int textureIndex = textureX * TEXTURE_ATLAS_WIDTH + textureY;
    for (int i = 0; i < 20; i += 5) {
        std::bitset<5> xPosBits(faceVertices[i] + blockPosInChunk.x);
        std::bitset<5> yPosBits(faceVertices[i + 1] + blockPosInChunk.y);
        std::bitset<9> zPosBits(faceVertices[i + 2] + blockPosInChunk.z);
        std::bitset<3> occlusionLevel(occlusionLevels[i / 5]);
        std::bitset<1> uBit(faceVertices[i + 3]);
        std::bitset<1> vBit(faceVertices[i + 4]);
        std::bitset<8> textureIndexBits(textureIndex);

        uint32_t encodedValue =
                (xPosBits.to_ulong() << 27) |
                (yPosBits.to_ulong() << 22) |
                (zPosBits.to_ulong() << 13) |
                (occlusionLevel.to_ulong() << 10) |
                (uBit.to_ulong() << 9) |
                (vBit.to_ulong() << 8) |
                (textureIndexBits.to_ulong() << 0);

        vertices.push_back(encodedValue);
    }

    // check whether to flip quad based on AO
    if (occlusionLevels[0] + occlusionLevels[3] > occlusionLevels[1] + occlusionLevels[2]) {
        indices.push_back(baseVertexIndex + 2);
        indices.push_back(baseVertexIndex + 0);
        indices.push_back(baseVertexIndex + 3);
        indices.push_back(baseVertexIndex + 3);
        indices.push_back(baseVertexIndex + 0);
        indices.push_back(baseVertexIndex + 1);
    } else {
        indices.push_back(baseVertexIndex);
        indices.push_back(baseVertexIndex + 1);
        indices.push_back(baseVertexIndex + 2);
        indices.push_back(baseVertexIndex + 2);
        indices.push_back(baseVertexIndex + 1);
        indices.push_back(baseVertexIndex + 3);
    }
}

void ChunkMesh::clearData() {
    vertices.clear();
    indices.clear();
}

void ChunkMesh::clearBuffers() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
    isBuffered = false;
}

OcclusionLevels
ChunkMesh::getOcclusionLevels(glm::ivec3 &blockPosInChunk, BlockFace face, const Ref<Chunk> &chunk,
                              ChunkManager &chunkManager) {

    // source: https://0fps.net/2013/07/03/ambient-occlusion-for-minecraft-like-worlds/

    OcclusionLevels occlusionLevels = NO_OCCLUSION;
    auto &faceLightingAdjacencies = lightingAdjacencies[static_cast<int>(face)];
    for (int faceVertexIndex = 0; faceVertexIndex < 4; faceVertexIndex++) {
        auto &faceLightingAdjacency = faceLightingAdjacencies[faceVertexIndex];
        bool side1IsSolid = false;
        bool side2IsSolid = false;
        bool cornerIsSolid = false;
        // side 1
        glm::ivec3 side1Pos = blockPosInChunk + faceLightingAdjacency[0];

        Block side1Block = chunk->getBlock(side1Pos, chunkManager);
        if (side1Block != Block::AIR) {
            side1IsSolid = true;
        }
        // side 2
        glm::ivec3 side2Pos = blockPosInChunk + faceLightingAdjacency[1];

        Block side2Block = chunk->getBlock(side2Pos, chunkManager);
        if (side2Block != Block::AIR) {
            side2IsSolid = true;
        }
        // corner
        glm::ivec3 cornerPos = blockPosInChunk + faceLightingAdjacency[2];
        Block cornerBlock = chunk->getBlock(cornerPos, chunkManager);
        if (cornerBlock != Block::AIR) {
            cornerIsSolid = true;
        }

        if (side1IsSolid && side2IsSolid) {
            occlusionLevels[faceVertexIndex] = 0;
        } else {
            occlusionLevels[faceVertexIndex] =
                    3 - (side1IsSolid + side2IsSolid + cornerIsSolid);
        }
    }

    return occlusionLevels;
}
