//
// Created by Tony Adriansen on 12/28/2023.
//

#include "ChunkMeshBuilder.hpp"
#include "../block/BlockDB.hpp"
#include "Chunk.hpp"

namespace {

constexpr int ALL_FACES_LOOKUP[120] = {
    // front
    1, 0, 0, 0, 0,  // bottom left
    1, 1, 0, 1, 0,  // bottom right
    1, 0, 1, 0, 1,  // top left
    1, 1, 1, 1, 1,  // top right

    // back
    0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1,

    // right
    0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1,

    // left
    0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1,

    // top
    0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1,

    // bottom
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

//  neighbor block array structure
// z
// |
// |  6   15  24
// |    7   16  25
// |      8   17  26
// |
// |  3   12  21
// |    4   13  22
// |      5   14  23
// \-------------------y
//  \ 0   9   18
//   \  1   10  19
//    \   2   11  20
//     x
// Bottom Left, Bottom right, Top Left, Top Right, corner is middle block in each 3 pairing
constexpr uint8_t LIGHTING_LOOKUP[6][4][3] = {
    // front face
    {{11, 2, 5}, {11, 20, 23}, {5, 8, 17}, {17, 26, 23}},
    // back face
    {{9, 18, 21}, {9, 0, 3}, {15, 6, 3}, {15, 24, 21}},
    // right face
    {{19, 20, 23}, {19, 18, 21}, {25, 26, 23}, {25, 24, 21}},
    // left face
    {{1, 0, 3}, {1, 2, 5}, {7, 6, 3}, {7, 8, 5}},
    // top face
    {{17, 8, 7}, {17, 26, 25}, {15, 6, 7}, {15, 24, 25}},
    // bottom face
    {{9, 0, 1}, {9, 18, 19}, {11, 2, 1}, {11, 20, 19}},
};


ChunkMeshBuilder::ChunkMeshBuilder(Block (&blocks)[CHUNK_MESH_INFO_SIZE],
                                   glm::ivec3 (&lightLevels)[CHUNK_VOLUME],
                                   const glm::ivec3 &chunkWorldPos) : m_blocks(
    blocks), m_chunkWorldPos(chunkWorldPos), m_lightLevels(lightLevels) {
}

void ChunkMeshBuilder::constructMesh(std::vector<ChunkVertex> &opaqueVertices,
                                     std::vector<unsigned int> &opaqueIndices,
                                     std::vector<ChunkVertex> &transparentVertices,
                                     std::vector<unsigned int> &transparentIndices) {
  ChunkVertex opaqueVertices_[20000];
  ChunkVertex transparentVertices_[20000];
  unsigned int opaqueIndices_[20000];
  unsigned int transparentIndices_[20000];
  int occlusionLevels[4];


  int opaqueVerticesIndex = 0;
  int transparentVerticesIndex = 0;
  int opaqueIndicesIndex = 0;
  int transparentIndicesIndex = 0;

  int chunkBaseZ = m_chunkWorldPos.z;
  int chunkBaseZIndex = chunkBaseZ / CHUNK_SIZE;
  int x, y, z, faceIndex;

  for (int chunkBlockIndex = 0; chunkBlockIndex < CHUNK_VOLUME; chunkBlockIndex++) {
    x = chunkBlockIndex & 31;
    y = (chunkBlockIndex >> 5) & 31;
    z = chunkBlockIndex >> 10;
    uint32_t blockIndex = MESH_XYZ(x, y, z);
    Block block = m_blocks[blockIndex];
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
      if (chunkBaseZIndex == 0 && adjBlockZ < 0) continue;
      if (chunkBaseZIndex == CHUNKS_PER_STACK - 1 && adjBlockZ >= CHUNK_SIZE) continue;
//      if (adjBlockZ + chunkBaseZ < 0 || adjBlockZ + chunkBaseZ >= WORLD_HEIGHT_BLOCKS) continue;

      uint32_t adjBlockIndex = MESH_XYZ(adjBlockX, adjBlockY, adjBlockZ);
      Block adjacentBlock = m_blocks[adjBlockIndex];
      if (adjacentBlock == block) continue;

      BlockData &adjBlockData = BlockDB::getBlockData(adjacentBlock);
      if (!adjBlockData.isTransparent) continue;

      auto face = static_cast<BlockFace>(faceIndex);

      BlockData &blockData = BlockDB::getBlockData(block);
      const glm::ivec3 &blockLightLevel = m_lightLevels[blockIndex];

      setOcclusionLevels(blockPos, face, occlusionLevels);
      int texIndex = blockData.texIndex[faceIndex];

      const int faceVerticesIndex = faceIndex * 20;

      auto vertices = blockData.isTransparent ? transparentVertices_ : opaqueVertices_;
      auto indices = blockData.isTransparent ? transparentIndices_ : opaqueIndices_;
      auto &verticesIndex = blockData.isTransparent ? transparentVerticesIndex : opaqueVerticesIndex;
      auto &indicesIndex = blockData.isTransparent ? transparentIndicesIndex : opaqueIndicesIndex;
      auto baseVertexIndex = verticesIndex;

      for (int i = 0, j = 0; i < 20; i += 5, j++) {
        // x between [0, 32] == 6 bits, y between [0, 32] == 6 bits, z
        // between [0, 32] == 6 bits occlusion level [0, 3] == 2 bits,
        // textureX [0, 1] == 1 bit, textureY [0, 1] == 1 bit textureIndex
        // [0, 255] == 8 bits. total 30 bits pack x, y, z, occlusion level,
        // textureX, textureY, textureIndex into 32 bits
        uint32_t vertexData1 =
            ((blockPos.x + ALL_FACES_LOOKUP[faceVerticesIndex + i]) & 0x3F) |
            ((blockPos.y + ALL_FACES_LOOKUP[faceVerticesIndex + i + 1] & 0x3F) << 6) |
            ((blockPos.z + ALL_FACES_LOOKUP[faceVerticesIndex + i + 2] & 0x3F) << 12) |
            ((occlusionLevels[j] & 0x3) << 18) |
            ((ALL_FACES_LOOKUP[faceVerticesIndex + i + 3] & 0x1) << 20) |
            ((ALL_FACES_LOOKUP[faceVerticesIndex + i + 4] & 0x1) << 21) |
            ((texIndex & 0xFF) << 22);
        // blue light level [0, 15] == 4 bits, green light level [0, 15] == 4, red light level [0, 15] == 4 bits
        // intensity [0, 15] == 4 bits, total 16 bits
//        int redLightLevel = 15;
//        int greenLightLevel = 0;
//        int blueLightLevel =  0;
        int intensity = 15;
        uint32_t vertexData2 =
            (blockLightLevel.b & 0xF) | ((blockLightLevel.g & 0xF) << 4) | ((blockLightLevel.r & 0xF) << 8) |
            ((intensity & 0xF) << 12);
        ChunkVertex vertex = {vertexData1, vertexData2};
        vertices[verticesIndex++] = vertex;
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
  opaqueVertices = std::vector<ChunkVertex>(opaqueVertices_, opaqueVertices_ + opaqueVerticesIndex);
  opaqueIndices = std::vector<unsigned int>(opaqueIndices_, opaqueIndices_ + opaqueIndicesIndex);
  transparentVertices = std::vector<ChunkVertex>(transparentVertices_, transparentVertices_ + transparentVerticesIndex);
  transparentIndices = std::vector<unsigned int>(transparentIndices_, transparentIndices_ + transparentIndicesIndex);
}

void ChunkMeshBuilder::setOcclusionLevels(const glm::ivec3 &blockPosInChunk,
                                          BlockFace face, int (&levels)[4]) {
  // source:
  // https://0fps.net/2013/07/03/ambient-occlusion-for-minecraft-like-worlds/


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
      levels[faceVertexIndex] = 0;
    } else {
      levels[faceVertexIndex] = 3 - (side1IsSolid + side2IsSolid + cornerIsSolid);
    }
  }
}

//void ChunkMeshBuilder::setOcclusionLevels(bool (&solidNeighborBlocks)[27], int faceIndex, int (&levels)[4]) {
//  bool aoSolidNeighbors[3];
//  for (int vertexIndex = 0; vertexIndex < 4; vertexIndex++) {
//    for (int blockToCheckIndex = 0; blockToCheckIndex < 3; blockToCheckIndex++) {
//      aoSolidNeighbors[blockToCheckIndex] = solidNeighborBlocks[LIGHTING_LOOKUP[faceIndex][vertexIndex][blockToCheckIndex]];
//    }
//    levels[vertexIndex] = aoSolidNeighbors[0] && aoSolidNeighbors[2] ? 0 : 3 -
//                                                                           (aoSolidNeighbors[0] + aoSolidNeighbors[1] +
//                                                                            aoSolidNeighbors[2]);
//  }
//}
//
