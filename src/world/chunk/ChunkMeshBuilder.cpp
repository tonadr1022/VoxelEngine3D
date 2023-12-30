//
// Created by Tony Adriansen on 12/28/2023.
//

#include "ChunkMeshBuilder.hpp"
#include "../block/BlockDB.hpp"
#include "Chunk.hpp"

namespace {
constexpr std::array<int, 20> frontFace{
    1, 0, 0, 0, 0,  // bottom left
    1, 1, 0, 1, 0,  // bottom right
    1, 0, 1, 0, 1,  // top left
    1, 1, 1, 1, 1,  // top right
};

constexpr std::array<int, 20> backFace{
    0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1,
};

constexpr std::array<int, 20> leftFace{
    0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1,
};

constexpr std::array<int, 20> rightFace{
    0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1,
};

constexpr std::array<int, 20> topFace{
    0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1,
};

constexpr std::array<int, 20> bottomFace{
    0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1,
};

constexpr std::array<std::array<std::array<glm::ivec3, 3>, 4>, 6>
    lightingAdjacencies = {{
                               // front face
                               {{
                                    // Bottom Left
                                    {glm::ivec3(1, 0, -1), glm::ivec3(1, -1, 0),
                                     glm::ivec3(1, -1, -1)},
                                    // Bottom Right
                                    {glm::ivec3(1, 0, -1), glm::ivec3(1, 1, 0),
                                     glm::ivec3(1, 1, -1)},
                                    // Top Left
                                    {glm::ivec3(1, 0, 1), glm::ivec3(1, -1, 0),
                                     glm::ivec3(1, -1, 1)},
                                    // Top Right
                                    {glm::ivec3(1, 0, 1), glm::ivec3(1, 1, 0),
                                     glm::ivec3(1, 1, 1)},
                                }},
                               // back face
                               {{
                                    // 0, 0, 0
                                    {glm::ivec3(-1, 0, -1),
                                     glm::ivec3(-1, -1, 0),
                                     glm::ivec3(-1, -1, -1)},
                                    // 0, 0, 1
                                    {glm::ivec3(-1, 0, 1),
                                     glm::ivec3(-1, -1, 0),
                                     glm::ivec3(-1, -1, 1)},
                                    // 0, 1, 0
                                    {glm::ivec3(-1, 0, -1),
                                     glm::ivec3(-1, 1, 0),
                                     glm::ivec3(-1, 1, -1)},
                                    // 0, 1, 1
                                    {glm::ivec3(-1, 1, 0), glm::ivec3(-1, 0, 1),
                                     glm::ivec3(-1, 1, 1)},
                                }},
                               // left face
                               {{
                                    // 0, 0, 0
                                    {glm::ivec3(0, -1, -1),
                                     glm::ivec3(-1, -1, 0),
                                     glm::ivec3(-1, -1, -1)},
                                    // 1, 0, 0
                                    {glm::ivec3(0, -1, -1),
                                     glm::ivec3(1, -1, 0),
                                     glm::ivec3(1, -1, -1)},
                                    // 0, 0, 1
                                    {glm::ivec3(-1, -1, 0),
                                     glm::ivec3(0, -1, 1),
                                     glm::ivec3(-1, -1, 1)},
                                    // 1, 0, 1
                                    {glm::ivec3(0, -1, 1), glm::ivec3(1, -1, 0),
                                     glm::ivec3(1, -1, 1)},
                                }},
                               // right face
                               {{
                                    // 0, 1, 0
                                    {glm::ivec3(0, 1, -1), glm::ivec3(-1, 1, 0),
                                     glm::ivec3(-1, 1, -1)},
                                    // 0, 1, 1
                                    {glm::ivec3(0, 1, 1), glm::ivec3(-1, 1, 0),
                                     glm::ivec3(-1, 1, 1)},
                                    // 1, 1, 0
                                    {glm::ivec3(0, 1, -1), glm::ivec3(1, 1, 0),
                                     glm::ivec3(1, 1, -1)},
                                    // 1, 1, 1
                                    {glm::ivec3(0, 1, 1), glm::ivec3(1, 1, 0),
                                     glm::ivec3(1, 1, 1)},
                                }},
                               // top face
                               {{
                                    // 0, 0, 1
                                    {glm::ivec3(0, -1, 1), glm::ivec3(-1, 0, 1),
                                     glm::ivec3(-1, -1, 1)},
                                    // 1, 0, 1
                                    {glm::ivec3(0, -1, 1), glm::ivec3(1, 0, 1),
                                     glm::ivec3(1, -1, 1)},
                                    // 0, 1, 1
                                    {glm::ivec3(0, 1, 1), glm::ivec3(-1, 0, 1),
                                     glm::ivec3(-1, 1, 1)},
                                    // 1, 1, 1
                                    {glm::ivec3(0, 1, 1), glm::ivec3(1, 0, 1),
                                     glm::ivec3(1, 1, 1)},
                                }},

                               // bottom face
                               {{
                                    // 0, 0, 0
                                    {glm::ivec3(0, -1, -1),
                                     glm::ivec3(-1, 0, -1),
                                     glm::ivec3(-1, -1, -1)},
                                    // 0, 1, 0
                                    {glm::ivec3(0, 1, -1),
                                     glm::ivec3(-1, 0, -1),
                                     glm::ivec3(-1, 1, -1)},
                                    // 1, 0, 0
                                    {glm::ivec3(0, -1, -1),
                                     glm::ivec3(1, 0, -1),
                                     glm::ivec3(1, -1, -1)},
                                    // 1, 1, 0
                                    {glm::ivec3(0, 1, -1), glm::ivec3(1, 0, -1),
                                     glm::ivec3(1, 1, -1)},
                                }},
                           }};
}  // namespace

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

Block ChunkMeshBuilder::getBlock(int x, int y, int z) {
  Block block;
  // adj block in -1, -1 chunk (Chunk 0)
  if (x < 0 && y < 0) {
    block = m_chunk0.getBlock(CHUNK_WIDTH - 1, CHUNK_WIDTH - 1, z);
  }
    // adj block in -1, 1 chunk (Chunk 2)
  else if (x < 0 && y >= CHUNK_WIDTH) {
    block = m_chunk2.getBlock(CHUNK_WIDTH - 1, 0, z);
  }

    // adj block in 1, -1 chunk (Chunk 6)
  else if (x >= CHUNK_WIDTH && y < 0) {
    block = m_chunk6.getBlock(0, CHUNK_WIDTH - 1, z);
  }

    // adj block in 1, 1 chunk (Chunk 8)
  else if (x >= CHUNK_WIDTH && y >= CHUNK_WIDTH) {
    block = m_chunk8.getBlock(0, 0, z);
  }

    // adj block in -1, 0 chunk (Chunk 1)
  else if (x < 0) {
    block = m_chunk1.getBlock(CHUNK_WIDTH - 1, y, z);
  }

    // adj block in 1, 0 chunk (Chunk 7)
  else if (x >= CHUNK_WIDTH) {
    block = m_chunk7.getBlock(0, y, z);
  }

    // adj block in 0, -1 chunk (Chunk 3)
  else if (y < 0) {
    block = m_chunk3.getBlock(x, CHUNK_WIDTH - 1, z);
  }

    // adj block in 0, 1 chunk (Chunk 5)
  else if (y >= CHUNK_WIDTH) {
    block = m_chunk5.getBlock(x, 0, z);
  }
    // in middle chunk (mesh building chunk)
  else {
    block = m_chunk4.getBlock(x, y, z);
  }

  return block;
}

ChunkMeshBuilder::ChunkMeshBuilder(const Chunk &chunk0, const Chunk &chunk1, const Chunk &chunk2, const Chunk &chunk3,
                                   const Chunk &chunk4, const Chunk &chunk5, const Chunk &chunk6, const Chunk &chunk7,
                                   const Chunk &chunk8)
    : m_chunk0(chunk0), m_chunk1(chunk1), m_chunk2(chunk2), m_chunk3(chunk3),
      m_chunk4(chunk4), m_chunk5(chunk5), m_chunk6(chunk6), m_chunk7(chunk7),
      m_chunk8(chunk8) {
}

/*
 * Neighbor Chunks Array Structure
 *
 * \------------------ x
 *  \  0  3  6
 *   \  1  4  7
 *    \  2  5  8
 *     y
 */
void ChunkMeshBuilder::constructMesh(std::vector<uint32_t> &opaqueVertices,
                                     std::vector<unsigned int> &opaqueIndices,
                                     std::vector<uint32_t> &transparentVertices,
                                     std::vector<unsigned int> &transparentIndices) {
  if (m_chunk0.chunkState != ChunkState::FULLY_GENERATED ||
      m_chunk1.chunkState != ChunkState::FULLY_GENERATED || m_chunk2.chunkState != ChunkState::FULLY_GENERATED
      || m_chunk3.chunkState != ChunkState::FULLY_GENERATED || m_chunk4.chunkState != ChunkState::FULLY_GENERATED
      || m_chunk5.chunkState != ChunkState::FULLY_GENERATED || m_chunk6.chunkState != ChunkState::FULLY_GENERATED
      || m_chunk7.chunkState != ChunkState::FULLY_GENERATED || m_chunk8.chunkState != ChunkState::FULLY_GENERATED) {
    return;
  }

  opaqueVertices.reserve(5000);
  opaqueIndices.reserve(5000);
  transparentVertices.reserve(5000);
  transparentIndices.reserve(5000);

  int x, y, z, faceIndex, textureX, textureY;
  std::array<int, 20> faceVertices{};

  AdjacentBlockPositions adjacentBlockPositions{};
  // TODO: fix level 255. random blocks appear, so data isn't set properly for
  // it
  for (z = 0; z < CHUNK_HEIGHT - 1; z++) {
    for (x = 0; x < CHUNK_WIDTH; x++) {
      for (y = 0; y < CHUNK_WIDTH; y++) {
        Block block = m_chunk4.getBlock(x, y, z);
        if (block == Block::AIR) continue;
        adjacentBlockPositions.update(x, y, z);
        glm::ivec3 blockPos = {x, y, z};
        for (faceIndex = 0; faceIndex < 6; faceIndex++) {
          glm::ivec3 adjacentBlockPos =
              adjacentBlockPositions.positions[faceIndex];
          if (adjacentBlockPos.z < 0 || adjacentBlockPos.z >= CHUNK_HEIGHT)
            continue;

          Block adjacentBlock = getBlock(adjacentBlockPos.x,
                                         adjacentBlockPos.y,
                                         adjacentBlockPos.z);

          BlockData &adjBlockData = BlockDB::getBlockData(adjacentBlock);
          if (!adjBlockData.isTransparent) continue;
          if (adjacentBlock == block) continue;

          auto face = static_cast<BlockFace>(faceIndex);

          BlockData &blockData = BlockDB::getBlockData(block);
          OcclusionLevels occlusionLevels = getOcclusionLevels(blockPos, face);
          switch (face) {
            case BlockFace::FRONT:faceVertices = frontFace;
              textureX = blockData.frontTexCoords.x;
              textureY = blockData.frontTexCoords.y;
              break;
            case BlockFace::BACK:faceVertices = backFace;
              textureX = blockData.backTexCoords.x;
              textureY = blockData.backTexCoords.y;
              break;
            case BlockFace::LEFT:faceVertices = leftFace;
              textureX = blockData.leftTexCoords.x;
              textureY = blockData.leftTexCoords.y;
              break;
            case BlockFace::RIGHT:faceVertices = rightFace;
              textureX = blockData.rightTexCoords.x;
              textureY = blockData.rightTexCoords.y;
              break;
            case BlockFace::TOP:faceVertices = topFace;
              textureX = blockData.topTexCoords.x;
              textureY = blockData.topTexCoords.y;
              break;
            case BlockFace::BOTTOM:faceVertices = bottomFace;
              textureX = blockData.bottomTexCoords.x;
              textureY = blockData.bottomTexCoords.y;
              break;
            default:break;
          }
          auto &vertices = blockData.isTransparent ? transparentVertices : opaqueVertices;
          auto &indices = blockData.isTransparent ? transparentIndices : opaqueIndices;

          auto baseVertexIndex = vertices.size();
          int textureIndex = textureX * TEXTURE_ATLAS_WIDTH + textureY;
          for (int i = 0; i < 20; i += 5) {
            // x between [0, 16] == 5 bits, y between [0, 16] == 5 bits, z between [0, 255] == 8 bits
            // occlusion level [0, 3] == 2 bits, textureX [0, 1] == 1 bit, textureY [0, 1] == 1 bit
            // textureIndex [0, 255] == 8 bits. total 30 bits
            // pack x, y, z, occlusion level, textureX, textureY, textureIndex into 32 bits
            uint32_t vertexData = ((blockPos.x + faceVertices[i]) & 0x1F) |
                ((blockPos.y + faceVertices[i + 1] & 0x1F) << 5) |
                ((blockPos.z + faceVertices[i + 2] & 0xFF) << 10) |
                ((occlusionLevels[i / 5] & 0x3) << 18) |
                ((faceVertices[i + 3] & 0x1) << 20) |
                ((faceVertices[i + 4] & 0x1) << 21) |
                ((textureIndex & 0xFF) << 22);
            vertices.emplace_back(vertexData);
          }

          // check whether to flip quad based on AO
          if (occlusionLevels[0] + occlusionLevels[3] >
              occlusionLevels[1] + occlusionLevels[2]) {
            indices.emplace_back(baseVertexIndex + 2);
            indices.emplace_back(baseVertexIndex + 0);
            indices.emplace_back(baseVertexIndex + 3);
            indices.emplace_back(baseVertexIndex + 3);
            indices.emplace_back(baseVertexIndex + 0);
            indices.emplace_back(baseVertexIndex + 1);
          } else {
            indices.emplace_back(baseVertexIndex);
            indices.emplace_back(baseVertexIndex + 1);
            indices.emplace_back(baseVertexIndex + 2);
            indices.emplace_back(baseVertexIndex + 2);
            indices.emplace_back(baseVertexIndex + 1);
            indices.emplace_back(baseVertexIndex + 3);
          }
        }
      }
    }
  }
  opaqueVertices.shrink_to_fit();
  transparentVertices.shrink_to_fit();
  opaqueIndices.shrink_to_fit();
  transparentIndices.shrink_to_fit();
}

OcclusionLevels ChunkMeshBuilder::getOcclusionLevels(const glm::ivec3 &blockPosInChunk,
                                                     BlockFace face) {
  // source:
  // https://0fps.net/2013/07/03/ambient-occlusion-for-minecraft-like-worlds/

  OcclusionLevels occlusionLevels = {3, 3, 3, 3};

  auto &faceLightingAdjacencies = lightingAdjacencies[static_cast<int>(face)];
  for (int faceVertexIndex = 0; faceVertexIndex < 4; faceVertexIndex++) {
    auto &faceLightingAdjacency = faceLightingAdjacencies[faceVertexIndex];
    bool side1IsSolid = false;
    bool side2IsSolid = false;
    bool cornerIsSolid = false;

    glm::ivec3 side1Pos = blockPosInChunk + faceLightingAdjacency[0];
    Block side1Block = getBlock(side1Pos.x, side1Pos.y, side1Pos.z);
    if (side1Block != Block::AIR) {
      side1IsSolid = true;
    }

    glm::ivec3 side2Pos = blockPosInChunk + faceLightingAdjacency[1];
    Block side2Block = getBlock(side2Pos.x, side2Pos.y, side2Pos.z);
    if (side2Block != Block::AIR) {
      side2IsSolid = true;
    }

    glm::ivec3 cornerPos = blockPosInChunk + faceLightingAdjacency[2];
    Block cornerBlock = getBlock(cornerPos.x, cornerPos.y, cornerPos.z);
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


