//
// Created by Tony Adriansen on 11/16/23.
//

#include "ChunkMesh.h"
#include "Chunk.h"
#include "../block/BlockDB.h"
#include <bitset>
#include <glad/glad.h>


namespace {
    constexpr std::array<int, 20> frontFace{
            1, 0, 0, 0, 0,
            1, 1, 0, 1, 0,
            1, 0, 1, 0, 1,
            1, 1, 1, 1, 1,
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
} // namespace


struct AdjacentBlockPositions {
    glm::ivec3 positions[static_cast<int>(BlockFace::COUNT)];

    void update(int x, int y, int z) {
        positions[static_cast<int>(BlockFace::TOP)] = {x, y, z + 1};
        positions[static_cast<int>(BlockFace::BOTTOM)] = {x, y, z - 1};
        positions[static_cast<int>(BlockFace::LEFT)] = {x, y - 1, z};
        positions[static_cast<int>(BlockFace::RIGHT)] = {x, y + 1, z};
        positions[static_cast<int>(BlockFace::FRONT)] = {x + 1, y, z};
        positions[static_cast<int>(BlockFace::BACK)] = {x - 1, y, z};
    }
};

ChunkMesh::ChunkMesh() : VAO(0), VBO(0), EBO(0) {
}

bool
ChunkMesh::shouldAddFace(glm::ivec3 &adjacentBlockPosInChunk, BlockFace face, Chunk &chunk,
                         Chunk &leftNeighborChunk,
                         Chunk &rightNeighborChunk, Chunk &frontNeighborChunk,
                         Chunk &backNeighborChunk) {
    // below min height
    if (adjacentBlockPosInChunk.z < 0) return false;
    // check if adjacent block is vertically out of bounds, if so add face
    if (adjacentBlockPosInChunk.z > CHUNK_HEIGHT) {
        return true;
    }

    if (adjacentBlockPosInChunk.x < 0) {
        // get back neighbor chunk
        if (backNeighborChunk.getBlock(CHUNK_WIDTH - 1, adjacentBlockPosInChunk.y,
                                       adjacentBlockPosInChunk.z).id == Block::AIR) {
            return true;
        }
    } else if (adjacentBlockPosInChunk.x >= CHUNK_WIDTH) {
        // get front neighbor chunk
        if (frontNeighborChunk.getBlock(0, adjacentBlockPosInChunk.y,
                                        adjacentBlockPosInChunk.z).id == Block::AIR) {
            return true;
        }
    } else if (adjacentBlockPosInChunk.y < 0) {
        // get left neighbor chunk
        if (leftNeighborChunk.getBlock(adjacentBlockPosInChunk.x, CHUNK_WIDTH - 1,
                                       adjacentBlockPosInChunk.z).id == Block::AIR) {
            return true;
        }
    } else if (adjacentBlockPosInChunk.y >= CHUNK_WIDTH) {
        // get right neighbor chunk
        if (rightNeighborChunk.getBlock(adjacentBlockPosInChunk.x, 0,
                                        adjacentBlockPosInChunk.z).id == Block::AIR) {
            return true;
        }
    } else {
        // check adjacent block (in chunk at this point), if it's air or transparent, add face
        Block adjacentBlock = chunk.getBlock(adjacentBlockPosInChunk.x, adjacentBlockPosInChunk.y,
                                             adjacentBlockPosInChunk.z);
        if (adjacentBlock.isTransparent()) {
            return true;
        }
    }
    return false;
}


void ChunkMesh::construct(Chunk &chunk, Chunk &leftNeighborChunk, Chunk &rightNeighborChunk,
                          Chunk &frontNeighborChunk,
                          Chunk &backNeighborChunk) {
    clearData();
    AdjacentBlockPositions adjacentBlockPositions{};
    for (Chunklet &chunklet: chunk.chunklets) {
        for (int chunkletZ = 0; chunkletZ < CHUNKLET_HEIGHT; chunkletZ++) {
            int chunkZ = static_cast<int>(chunklet.location.z) + chunkletZ;
            if (chunk.numSolidBlocksInLayers[chunkZ] == 0) {
                continue;
            }
            for (int x = 0; x < CHUNK_WIDTH; x++) {
                for (int y = 0; y < CHUNK_WIDTH; y++) {
                    int maxBlockHeight = chunk.getMaxBlockHeightAt(x, y);
                    if (chunkZ > maxBlockHeight) {
                        continue;
                    }
                    Block block = chunklet.getBlock(x, y, chunkletZ);
                    if (block.id == Block::AIR) {
                        continue;
                    }

                    // block pos in chunk
                    glm::ivec3 blockPosInChunk = {x, y, chunkZ};
                    adjacentBlockPositions.update(x, y, chunkZ);
                    for (int i = 0; i < static_cast<int>(BlockFace::COUNT); i++) {
                        auto face = static_cast<BlockFace>(i);
                        glm::ivec3 adjacentBlockPos = adjacentBlockPositions.positions[static_cast<int>(face)];
                        if (shouldAddFace(adjacentBlockPos, face, chunk, leftNeighborChunk,
                                          rightNeighborChunk, frontNeighborChunk,
                                          backNeighborChunk)) {
                            addFace(blockPosInChunk, block, face);
                        }
                    }
                }
            }
        }
    }
}


void ChunkMesh::addFace(glm::ivec3 &blockPosInChunk, Block &block, BlockFace face) {
    BlockData &blockData = BlockDB::getBlockData(block.id);
    int textureX = 0;
    int textureY = 0;
    std::array<int, 20> faceVertices{};
    int faceNum;
    switch (face) {
        case BlockFace::FRONT:
            faceNum = 0;
            faceVertices = frontFace;
            textureX = blockData.frontTexCoords.x;
            textureY = blockData.frontTexCoords.y;
            break;
        case BlockFace::BACK:
            faceNum = 1;
            faceVertices = backFace;
            textureX = blockData.backTexCoords.x;
            textureY = blockData.backTexCoords.y;
            break;
        case BlockFace::LEFT:
            faceNum = 2;
            faceVertices = leftFace;
            textureX = blockData.leftTexCoords.x;
            textureY = blockData.leftTexCoords.y;
            break;
        case BlockFace::RIGHT:
            faceNum = 3;
            faceVertices = rightFace;
            textureX = blockData.rightTexCoords.x;
            textureY = blockData.rightTexCoords.y;
            break;
        case BlockFace::TOP:
            faceNum = 4;
            faceVertices = topFace;
            textureX = blockData.topTexCoords.x;
            textureY = blockData.topTexCoords.y;
            break;
        case BlockFace::BOTTOM:
            faceNum = 5;
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
        std::bitset<3> faceBits(faceNum);
        std::bitset<1> uBit(faceVertices[i + 3]);
        std::bitset<1> vBit(faceVertices[i + 4]);
        std::bitset<8> textureIndexBits(textureIndex);

        uint32_t encodedValue =
                (xPosBits.to_ulong() << 27) |
                (yPosBits.to_ulong() << 22) |
                (zPosBits.to_ulong() << 13) |
                (faceBits.to_ulong() << 10) |
                (uBit.to_ulong() << 9) |
                (vBit.to_ulong() << 8) |
                (textureIndexBits.to_ulong() << 0);

        vertices.push_back(encodedValue);
    }
    indices.push_back(baseVertexIndex);
    indices.push_back(baseVertexIndex + 1);
    indices.push_back(baseVertexIndex + 2);
    indices.push_back(baseVertexIndex + 2);
    indices.push_back(baseVertexIndex + 1);
    indices.push_back(baseVertexIndex + 3);
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

