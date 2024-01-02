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
                                    {glm::ivec3(-1, 0, 1), glm::ivec3(-1, -1, 0),
                                     glm::ivec3(-1, -1, 1)},
                                    // 0, 1, 0
                                    {glm::ivec3(-1, 0, -1), glm::ivec3(-1, 1, 0),
                                     glm::ivec3(-1, 1, -1)},
                                    // 0, 1, 1
                                    {glm::ivec3(-1, 1, 0), glm::ivec3(-1, 0, 1), glm::ivec3(-1, 1, 1)},
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
                               // left face
                               {{
                                    // 0, 0, 0
                                    {glm::ivec3(0, -1, -1), glm::ivec3(-1, -1, 0),
                                     glm::ivec3(-1, -1, -1)},
                                    // 1, 0, 0
                                    {glm::ivec3(0, -1, -1), glm::ivec3(1, -1, 0),
                                     glm::ivec3(1, -1, -1)},
                                    // 0, 0, 1
                                    {glm::ivec3(-1, -1, 0), glm::ivec3(0, -1, 1),
                                     glm::ivec3(-1, -1, 1)},
                                    // 1, 0, 1
                                    {glm::ivec3(0, -1, 1), glm::ivec3(1, -1, 0), glm::ivec3(1, -1, 1)},
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
                                    {glm::ivec3(0, 1, -1), glm::ivec3(-1, 0, -1),
                                     glm::ivec3(-1, 1, -1)},
                                    // 1, 0, 0
                                    {glm::ivec3(0, -1, -1), glm::ivec3(1, 0, -1),
                                     glm::ivec3(1, -1, -1)},
                                    // 1, 1, 0
                                    {glm::ivec3(0, 1, -1), glm::ivec3(1, 0, -1), glm::ivec3(1, 1, -1)},
                                }},
                           }};
}  // namespace


ChunkMeshBuilder::ChunkMeshBuilder(Block (&blocks)[CHUNK_MESH_INFO_SIZE], const glm::ivec3 &chunkWorldPos) : m_blocks(
    blocks), m_chunkWorldPos(chunkWorldPos) {
}

void ChunkMeshBuilder::constructMesh(std::vector<uint32_t> &opaqueVertices,
                                     std::vector<unsigned int> &opaqueIndices,
                                     std::vector<uint32_t> &transparentVertices,
                                     std::vector<unsigned int> &transparentIndices) {
  uint32_t opaqueVertices_[20000];
  uint32_t transparentVertices_[20000];
  unsigned int opaqueIndices_[20000];
  unsigned int transparentIndices_[20000];

  int opaqueVerticesIndex = 0;
  int transparentVerticesIndex = 0;
  int opaqueIndicesIndex = 0;
  int transparentIndicesIndex = 0;

  int chunkBaseZ = m_chunkWorldPos.z;
  int x, y, z, faceIndex, textureX, textureY;
  std::array<int, 20> faceVertices{};

  for (int chunkBlockIndex = 0; chunkBlockIndex < CHUNK_VOLUME; chunkBlockIndex++) {
    y = chunkBlockIndex & 31;
    x = (chunkBlockIndex >> 5) & 31;
    z = chunkBlockIndex >> 10;

    Block block = m_blocks[MESH_XYZ(x, y, z)];
    if (block == Block::AIR) continue;

    glm::ivec3 blockPos = {x, y, z};
    int blockPos2[3] = {x, y, z};

    // 1, 0, 0
    // -1, 0, 0
    // 0, 1, 0
    // 0, -1, 0
    // 0, 0, 1
    // 0, 0, -1
    for (faceIndex = 0; faceIndex < 6; faceIndex++) {
      int adjBlockPos[3] = {blockPos2[0], blockPos2[1], blockPos2[2]};
      adjBlockPos[faceIndex >> 1] += 1 - ((faceIndex & 1) << 1);

      int adjBlockX = adjBlockPos[0];
      int adjBlockY = adjBlockPos[1];
      int adjBlockZ = adjBlockPos[2];


      // skip faces adjacent to absolute borders
      if (adjBlockZ + chunkBaseZ < 0 || adjBlockZ + chunkBaseZ >= WORLD_HEIGHT_BLOCKS) continue;

      Block adjacentBlock = m_blocks[MESH_XYZ(adjBlockX, adjBlockY, adjBlockZ)];
      if (adjacentBlock == block) continue;

      BlockData &adjBlockData = BlockDB::getBlockData(adjacentBlock);
      if (!adjBlockData.isTransparent) continue;

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

      auto vertices = blockData.isTransparent ? transparentVertices_ : opaqueVertices_;
      auto indices = blockData.isTransparent ? transparentIndices_ : opaqueIndices_;
      auto &verticesIndex = blockData.isTransparent ? transparentVerticesIndex : opaqueVerticesIndex;
      auto &indicesIndex = blockData.isTransparent ? transparentIndicesIndex : opaqueIndicesIndex;
      auto baseVertexIndex = verticesIndex;

      int textureIndex = textureX * TEXTURE_ATLAS_WIDTH + textureY;
      for (int i = 0, j = 0; i < 20; i += 5, j++) {
        // x between [0, 32] == 6 bits, y between [0, 32] == 6 bits, z
        // between [0, 32] == 6 bits occlusion level [0, 3] == 2 bits,
        // textureX [0, 1] == 1 bit, textureY [0, 1] == 1 bit textureIndex
        // [0, 255] == 8 bits. total 30 bits pack x, y, z, occlusion level,
        // textureX, textureY, textureIndex into 32 bits
        uint32_t vertexData =
            ((blockPos.x + faceVertices[i]) & 0x3F) |
                ((blockPos.y + faceVertices[i + 1] & 0x3F) << 6) |
                ((blockPos.z + faceVertices[i + 2] & 0x3F) << 12) |
                ((occlusionLevels[j] & 0x3) << 18) |
                ((faceVertices[i + 3] & 0x1) << 20) |
                ((faceVertices[i + 4] & 0x1) << 21) |
                ((textureIndex & 0xFF) << 22);
        vertices[verticesIndex++] = vertexData;
      }
      // check whether to flip quad based on AO
      if (occlusionLevels[0] + occlusionLevels[3] > occlusionLevels[1] + occlusionLevels[2]) {
        indices[indicesIndex++] = baseVertexIndex + 2;
        indices[indicesIndex++] = baseVertexIndex + 0;
        indices[indicesIndex++] = baseVertexIndex + 3;
        indices[indicesIndex++] = baseVertexIndex + 3;
        indices[indicesIndex++] = baseVertexIndex + 0;
        indices[indicesIndex++] = baseVertexIndex + 1;
      } else {
        indices[indicesIndex++] = baseVertexIndex;
        indices[indicesIndex++] = baseVertexIndex + 1;
        indices[indicesIndex++] = baseVertexIndex + 2;
        indices[indicesIndex++] = baseVertexIndex + 2;
        indices[indicesIndex++] = baseVertexIndex + 1;
        indices[indicesIndex++] = baseVertexIndex + 3;
      }
    }
  }
  opaqueVertices = std::vector<uint32_t>(opaqueVertices_, opaqueVertices_ + opaqueVerticesIndex);
  opaqueIndices = std::vector<unsigned int>(opaqueIndices_, opaqueIndices_ + opaqueIndicesIndex);
  transparentVertices = std::vector<uint32_t>(transparentVertices_, transparentVertices_ + transparentVerticesIndex);
  transparentIndices = std::vector<unsigned int>(transparentIndices_, transparentIndices_ + transparentIndicesIndex);
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
    Block side1Block = m_blocks[MESH_XYZ(side1Pos.x, side1Pos.y, side1Pos.z)];
    if (side1Block != Block::AIR) {
      side1IsSolid = true;
    }

    glm::ivec3 side2Pos = blockPosInChunk + faceLightingAdjacency[1];
    Block side2Block = m_blocks[MESH_XYZ(side2Pos.x, side2Pos.y, side2Pos.z)];
    if (side2Block != Block::AIR) {
      side2IsSolid = true;
    }

    glm::ivec3 cornerPos = blockPosInChunk + faceLightingAdjacency[2];
    Block cornerBlock = m_blocks[MESH_XYZ(cornerPos.x, cornerPos.y, cornerPos.z)];
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

