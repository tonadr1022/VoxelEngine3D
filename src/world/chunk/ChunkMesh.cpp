//
// Created by Tony Adriansen on 11/16/23.
//

#include "ChunkMesh.h"
#include "Chunk.h"

namespace {
    const std::array<GLfloat, 32> frontFace{
            1, 0, 0, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            1, 1, 0, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            1, 0, 1, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            1, 1, 1, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    };

    const std::array<GLfloat, 32> backFace{
            0, 0, 0, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            0, 0, 1, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            0, 1, 0, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            0, 1, 1, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    };

    const std::array<GLfloat, 32> leftFace{
            0, 0, 0, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
            1, 0, 0, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
            0, 0, 1, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
            1, 0, 1, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
    };

    const std::array<GLfloat, 32> rightFace{
            0, 1, 0, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0, 1, 1, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            1, 1, 0, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            1, 1, 1, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    };

    const std::array<GLfloat, 32> topFace{
            0, 0, 1, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            1, 0, 1, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0, 1, 1, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1, 1, 1, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };

    const std::array<GLfloat, 32> bottomFace{
            0, 0, 0, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
            0, 1, 0, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
            1, 0, 0, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
            1, 1, 0, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
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
//     check adjacent block (in chunk at this point), if it's air or transparent, add face
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
    auto start = std::chrono::high_resolution_clock::now();
    AdjacentBlockPositions adjacentBlockPositions{};
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_WIDTH; y++) {
            // get max block height z for this xy pos in the chunk
            int maxBlockHeight = chunk.getMaxBlockHeightAt(x, y);
            for (Chunklet &chunklet: chunk.chunklets) {
                for (int chunkletZ = 0; chunkletZ < CHUNKLET_HEIGHT; chunkletZ++) {
                    // continue past chunklet
                    if (chunklet.location.z + chunkletZ > maxBlockHeight) {
                        goto chunkletBreak;
                    }
                    Block block = chunklet.getBlock(x, y, chunkletZ);
                    if (block.id == Block::AIR) {
                        continue;
                    }

                    int chunkZ = static_cast<int>(chunklet.location.z) + chunkletZ;
                    if (chunk.getIsBlockBuried(x, y, chunkZ)) {
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
            chunkletBreak:;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    if (duration.count() > 0.5)
        std::cout << "Chunk generation took: " << duration.count() << "microseconds"
                  << std::endl;
}

void ChunkMesh::addFace(glm::ivec3 &blockPosInChunk, Block block, BlockFace face) {
    std::array<GLfloat, 32> faceVertices{};
    switch (face) {
        case BlockFace::FRONT:
            faceVertices = frontFace;
            break;
        case BlockFace::BACK:
            faceVertices = backFace;
            break;
        case BlockFace::LEFT:
            faceVertices = leftFace;
            break;
        case BlockFace::RIGHT:
            faceVertices = rightFace;
            break;
        case BlockFace::TOP:
            faceVertices = topFace;
            break;
        case BlockFace::BOTTOM:
            faceVertices = bottomFace;
            break;
        default:
            break;
    }
    for (int i = 0; i < 32; i += 8) {
        faceVertices[i] += static_cast<float>(blockPosInChunk.x);
        faceVertices[i + 1] += static_cast<float>(blockPosInChunk.y);
        faceVertices[i + 2] += static_cast<float>(blockPosInChunk.z);
    }
    vertices.insert(vertices.end(), faceVertices.begin(), faceVertices.end());
    unsigned int offset = static_cast<int>(vertices.size() / 8 - 4);
    indices.insert(indices.end(), {
            offset, offset + 1, offset + 2,
            offset + 2, offset + 1, offset + 3
    });
}

void ChunkMesh::destruct() {

    vertices.clear();
    indices.clear();
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
}

