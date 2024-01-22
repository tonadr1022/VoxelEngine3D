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
//constexpr uint8_t LIGHTING_LOOKUP[6][4][3] = {
//    // front face
//    {{11, 2, 5}, {11, 20, 23}, {5, 8, 17}, {17, 26, 23}},
//    // back face
//    {{9, 18, 21}, {9, 0, 3}, {15, 6, 3}, {15, 24, 21}},
//    // right face
//    {{19, 20, 23}, {19, 18, 21}, {25, 26, 23}, {25, 24, 21}},
//    // left face
//    {{1, 0, 3}, {1, 2, 5}, {7, 6, 3}, {7, 8, 5}},
//    // top face
//    {{17, 8, 7}, {17, 26, 25}, {15, 6, 7}, {15, 24, 25}},
//    // bottom face
//    {{9, 0, 1}, {9, 18, 19}, {11, 2, 1}, {11, 20, 19}},
//};
constexpr uint8_t LIGHTING_LOOKUP[6][4][3] = {
    // front face
    {{5, 8, 17}, {11, 2, 5}, {11, 20, 23}, {17, 26, 23}},
    // back face
    {{15, 24, 21}, {9, 18, 21}, {9, 0, 3}, {15, 6, 3}},
    // right face
    {{25, 26, 23}, {19, 20, 23}, {19, 18, 21}, {25, 24, 21}},
    // left face
    {{7, 6, 3}, {1, 0, 3}, {1, 2, 5}, {7, 8, 5}},

    // top face
    {{15, 6, 7}, {17, 8, 7}, {17, 26, 25}, {15, 24, 25}},
    // bottom face
    {{11, 2, 1}, {9, 0, 1}, {9, 18, 19}, {11, 20, 19}},
};

ChunkMeshBuilder::ChunkMeshBuilder(Block (&blocks)[CHUNK_MESH_INFO_SIZE],
                                   uint16_t (&lightLevels)[CHUNK_MESH_INFO_SIZE],
                                   uint8_t (&sunlightLevels)[CHUNK_MESH_INFO_SIZE],
                                   const glm::ivec3 &chunkWorldPos) : m_blocks(
    blocks), m_chunkWorldPos(chunkWorldPos), m_lightLevels(lightLevels), m_sunlightLevels(sunlightLevels) {
}

void ChunkMeshBuilder::constructMesh(std::vector<ChunkVertex> &opaqueVertices,
                                     std::vector<unsigned int> &opaqueIndices,
                                     std::vector<ChunkVertex> &transparentVertices,
                                     std::vector<unsigned int> &transparentIndices) {
  auto opaqueVertices_ = new std::vector<ChunkVertex>();
  auto opaqueIndices_ = new std::vector<unsigned int>();
  auto transparentVertices_ = new std::vector<ChunkVertex>();
  auto transparentIndices_ = new std::vector<unsigned int>();

  int occlusionLevels[4];

  int chunkBaseZ = m_chunkWorldPos.z;
  int chunkBaseZIndex = chunkBaseZ / CHUNK_SIZE;
  bool checkAbsBottomBorder = chunkBaseZIndex == 0;
  bool checkAbsTopBorder = chunkBaseZIndex == CHUNKS_PER_STACK - 1;

  int x, y, z, faceIndex;

  for (int chunkBlockIndex = 0; chunkBlockIndex < CHUNK_VOLUME; chunkBlockIndex++) {
    x = chunkBlockIndex & 31;
    y = (chunkBlockIndex >> 5) & 31;
    z = chunkBlockIndex >> 10;
    uint32_t meshBlockIndex = MESH_XYZ(x, y, z);
    Block block = m_blocks[meshBlockIndex];
    if (block == Block::AIR) continue;
//    int blockPos[3] = {x, y, z};
    glm::ivec3 blockPos = {x, y, z};
    BlockData &blockData = BlockDB::getBlockData(block);

    // 1, 0, 0
    // -1, 0, 0
    // 0, 1, 0
    // 0, -1, 0
    // 0, 0, 1
    // 0, 0, -1
    for (faceIndex = 0; faceIndex < 6; faceIndex++) {
      // get adj block position
      int adjBlockPos[3] = {blockPos.x, blockPos.y, blockPos.z};
      adjBlockPos[faceIndex >> 1] += 1 - ((faceIndex & 1) << 1);

      // skip faces adjacent to absolute borders
      if (checkAbsBottomBorder && adjBlockPos[2] < 0) continue;
      if (checkAbsTopBorder && adjBlockPos[2] >= CHUNK_SIZE) continue;

      const uint32_t adjBlockIndex = MESH_XYZ(adjBlockPos[0], adjBlockPos[1], adjBlockPos[2]);
      const Block adjacentBlock = m_blocks[adjBlockIndex];
      if (adjacentBlock == block) continue;

      const BlockData &adjBlockData = BlockDB::getBlockData(adjacentBlock);
      if (!adjBlockData.isTransparent) continue;

      const uint16_t faceLightLevel = m_lightLevels[adjBlockIndex];
      const uint8_t sunlightLevel = m_sunlightLevels[adjBlockIndex];
      setOcclusionLevels(blockPos, faceIndex, occlusionLevels);

      const int texIndex = blockData.texIndex[faceIndex];
      const int faceVerticesIndex = faceIndex * 20;

      auto &vertices = blockData.isTransparent ? transparentVertices_ : opaqueVertices_;
      auto &indices = blockData.isTransparent ? transparentIndices_ : opaqueIndices_;

      const auto baseVertexIndex = vertices->size();
      uint32_t vertexData2 = faceLightLevel | ((sunlightLevel) << 12) | ((texIndex & 0xFFF) << 16);
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
                ((ALL_FACES_LOOKUP[faceVerticesIndex + i + 3] & 0x3F) << 20) |
                ((ALL_FACES_LOOKUP[faceVerticesIndex + i + 4] & 0x3F) << 26);
        // blue light level [0, 15] == 4 bits, green light level [0, 15] == 4, red light level [0, 15] == 4 bits
        // intensity [0, 15] == 4 bits, total 16 bits
        ChunkVertex vertex = {vertexData1, vertexData2};
        vertices->push_back(vertex);
      }
      // check whether to flip quad based on AO
      if (occlusionLevels[0] + occlusionLevels[3] > occlusionLevels[1] + occlusionLevels[2]) {
        indices->push_back(baseVertexIndex + 2);
        indices->push_back(baseVertexIndex + 0);
        indices->push_back(baseVertexIndex + 3);
        indices->push_back(baseVertexIndex + 3);
        indices->push_back(baseVertexIndex + 0);
        indices->push_back(baseVertexIndex + 1);

      } else {
        indices->push_back(baseVertexIndex);
        indices->push_back(baseVertexIndex + 1);
        indices->push_back(baseVertexIndex + 2);
        indices->push_back(baseVertexIndex + 2);
        indices->push_back(baseVertexIndex + 1);
        indices->push_back(baseVertexIndex + 3);
      }
    }
  }
  opaqueVertices = *opaqueVertices_;
  opaqueIndices = *opaqueIndices_;
  transparentVertices = *transparentVertices_;
  transparentIndices = *transparentIndices_;
  delete opaqueVertices_;
  delete transparentVertices_;
  delete opaqueIndices_;
  delete transparentIndices_;
}

void ChunkMeshBuilder::setOcclusionLevels(const glm::ivec3 &blockPosInChunk,
                                          int faceIndex, int (&levels)[4]) {
  // source:
  // https://0fps.net/2013/07/03/ambient-occlusion-for-minecraft-like-worlds/


  auto &faceLightingAdjacencies = lightingAdjacencies[static_cast<int>(faceIndex)];
  for (int faceVertexIndex = 0; faceVertexIndex < 4; faceVertexIndex++) {
    auto &faceLightingAdjacency = faceLightingAdjacencies[faceVertexIndex];
    bool side1IsSolid = false;
    bool side2IsSolid = false;
    bool cornerIsSolid = false;

    glm::ivec3 side1Pos = blockPosInChunk + faceLightingAdjacency[0];
    if (m_blocks[MESH_XYZ(side1Pos.x, side1Pos.y, side1Pos.z)] != Block::AIR) {
      side1IsSolid = true;
    }

    glm::ivec3 side2Pos = blockPosInChunk + faceLightingAdjacency[1];
    if (m_blocks[MESH_XYZ(side2Pos.x, side2Pos.y, side2Pos.z)] != Block::AIR) {
      side2IsSolid = true;
    }

    glm::ivec3 cornerPos = blockPosInChunk + faceLightingAdjacency[2];
    if (m_blocks[MESH_XYZ(cornerPos.x, cornerPos.y, cornerPos.z)] != Block::AIR) {
      cornerIsSolid = true;
    }

    if (side1IsSolid && side2IsSolid) {
      levels[faceVertexIndex] = 0;
    } else {
      levels[faceVertexIndex] = 3 - (side1IsSolid + side2IsSolid + cornerIsSolid);
    }
  }
}

void ChunkMeshBuilder::constructMeshGreedy(std::vector<ChunkVertex> &opaqueVertices,
                                           std::vector<unsigned int> &opaqueIndices,
                                           std::vector<ChunkVertex> &transparentVertices,
                                           std::vector<unsigned int> &transparentIndices) {
  static float maxTime = 0;
  static int count = 0;
  auto startTime = std::chrono::high_resolution_clock::now();

  // get face data
//  FaceInfo faceInfo[CHUNK_VOLUME][6] = {}; // need to initialize in case skip faces, which will be left as 0
  auto faceInfo = new FaceInfo[CHUNK_VOLUME][6];

  Block blockNeighbors[27];
  uint8_t sunlightNeighbors[27];
  uint16_t torchlightNeighbors[27];
  for (int chunkBlockIndex = 0; chunkBlockIndex < CHUNK_VOLUME; chunkBlockIndex++) {

    // derive block position from the index
    int blockPos[3];
    blockPos[0] = chunkBlockIndex & 31; // X (right most 5 bits in the index value
    blockPos[1] = (chunkBlockIndex >> 5) & 31; // Y (middle 5 bits in the index value)
    blockPos[2] = chunkBlockIndex >> 10; // Z (left most 5 bits in the index value)

    // get block, if its air, none of the faces should be added
    Block block = m_blocks[MESH_XYZ(blockPos[0], blockPos[1], blockPos[2])];
    if (block == Block::AIR) continue; // air is 0

    // front, back, right, left, top, bottom
    bool neighborsInitializedForCurrentBlock = false;
    for (uint8_t faceNum = 0; faceNum < 6; faceNum++) {

      // calculate adjacent block position for current face based on current block position
      // offset for any given adjacent block position is 1, so
      //  5 >> 1 == 2, 4 >> 1 == 2, 3 >> 1 == 1, 2 >> 1 == 1, 1 >> 1 == 0, 0 >> 1 == 0
      // add 1 or -1 to the corresponding position based on whether faceNum is odd. iff odd, subtract 2 from 1 to get
      // overall offset of -1 for that axis.
      int adjBlockPos[3] = {blockPos[0], blockPos[1], blockPos[2]};
      adjBlockPos[faceNum >> 1] += 1 - ((faceNum & 1) << 1);

      // since greedy meshing, no longer need to check whether neighbor is above or below absolute height borders
      // this will also allow for infinite height impl in future.
      Block neighborBlock = m_blocks[MESH_XYZ(adjBlockPos[0], adjBlockPos[1], adjBlockPos[2])];
      if (!shouldShowFace(block, neighborBlock)) {
        if (((block == Block::OAK_LEAVES && neighborBlock == Block::BIRCH_LEAVES)
            || (block == Block::BIRCH_LEAVES && neighborBlock == Block::OAK_LEAVES))) {
          int sdf = 5;
        }
        continue;
      };

      uint16_t neighborTorchlightLevel = m_lightLevels[MESH_XYZ(adjBlockPos[0], adjBlockPos[1], adjBlockPos[2])];
      uint8_t neighborSunlightLevel = m_sunlightLevels[MESH_XYZ(adjBlockPos[0], adjBlockPos[1], adjBlockPos[2])];

      // fill neighbors for block if haven't. only do this on first iteration of face loop after at least one face should be shown.
      if (!neighborsInitializedForCurrentBlock) {
        int neighborIndex = 0;
        int iterator[3];

        // see neighbor array offsets in World.hpp
        for (iterator[1] = blockPos[1] - 1; iterator[1] <= blockPos[1] + 1; ++iterator[1]) {
          for (iterator[2] = blockPos[2] - 1; iterator[2] <= blockPos[2] + 1; ++iterator[2]) {
            for (iterator[0] = blockPos[0] - 1; iterator[0] <= blockPos[0] + 1; ++iterator[0]) {
              blockNeighbors[neighborIndex] = m_blocks[MESH_XYZ(iterator[0], iterator[1], iterator[2])];
              torchlightNeighbors[neighborIndex] = m_lightLevels[MESH_XYZ(iterator[0], iterator[1], iterator[2])];
              sunlightNeighbors[neighborIndex] = m_sunlightLevels[MESH_XYZ(iterator[0], iterator[1], iterator[2])];
              neighborIndex++;
            }
          }
        }
        neighborsInitializedForCurrentBlock = true;
      }

      // with neighbors and knowledge that this face will be added to the mesh, add its face data to the array
      faceInfo[chunkBlockIndex][faceNum].setValues(faceNum,
                                                   blockNeighbors,
                                                   neighborTorchlightLevel,
                                                   neighborSunlightLevel);
    }
  }
  int u, v, counter, j, i, k, l, height, width;
  int faceNum;
  int x[3]; // start point
  int q[3]; // offset
  int du[3];
  int dv[3];
  FaceInfo *infoMask[CHUNK_AREA]; // use pointers since data already exists in the faceInfo array above
  Block mask[CHUNK_AREA];

  for (bool backFace = true, b = false; b != backFace; backFace = backFace && b, b = !b) {
    for (int axis = 0; axis < 3; axis++) {
      faceNum = ((axis << 1) | (backFace));

      u = (axis + 1) % 3;
      v = (axis + 2) % 3;

      x[0] = 0, x[1] = 0, x[2] = 0, x[axis] = -1;
      q[0] = 0, q[1] = 0, q[2] = 0, q[axis] = 1;

      // move through chunk from front to back
      while (x[axis] < CHUNK_SIZE) {
        counter = 0;
        for (x[v] = 0; x[v] < CHUNK_SIZE; x[v]++) {
          for (x[u] = 0; x[u] < CHUNK_SIZE; x[u]++, counter++) {
            const Block block1 = m_blocks[MESH_XYZ(x[0], x[1], x[2])];
            const Block block2 = m_blocks[MESH_XYZ(x[0] + q[0], x[1] + q[1], x[2] + q[2])];

            if (!backFace) {
              bool block1OutsideBorder = x[axis] < 0;
              if (!block1OutsideBorder && shouldShowFace(block1, block2)) {
                mask[counter] = block1;
                infoMask[counter] = &faceInfo[XYZ(x[0], x[1], x[2])][axis << 1];
              } else {
                infoMask[counter] = nullptr;
                mask[counter] = Block::AIR;
              }
            } else {
              bool block2OutsideBorder = CHUNK_SIZE - 1 <= x[axis];
              if (!block2OutsideBorder && shouldShowFace(block2, block1)) {
                mask[counter] = block2;
                infoMask[counter] = &faceInfo[XYZ(x[0] + q[0], x[1] + q[1], x[2] + q[2])][(axis << 1) | 1];
              } else {
                infoMask[counter] = nullptr;
                mask[counter] = Block::AIR;
              }
            }
          }
        }
        x[axis]++;

        // generate mesh for the mask
        counter = 0;

        for (j = 0; j < CHUNK_SIZE; j++) {
          for (i = 0; i < CHUNK_SIZE;) {
            Block block = mask[counter];
            if (block != Block::AIR) { // skip if air or zeroed
              FaceInfo &currFaceInfo = *infoMask[counter];
              // compute width
              for (width = 1; block == mask[counter + width] && infoMask[counter + width]
                  && currFaceInfo == *infoMask[counter + width] && i + width < CHUNK_SIZE; width++) {}

              // compute height
              bool done = false;
              for (height = 1; j + height < CHUNK_SIZE; height++) {
                for (k = 0; k < width; k++) {
                  int index = counter + k + height * CHUNK_SIZE;
                  if (block != mask[index] || !infoMask[index] || currFaceInfo != *infoMask[index]) {
                    done = true;
                    break;
                  }
                }
                if (done) break;
              }

              x[u] = i;
              x[v] = j;

              du[0] = 0, du[1] = 0, du[2] = 0;
              dv[0] = 0, dv[1] = 0, dv[2] = 0;

//              faceNum = ((axis << 1) | (blockType <= 0));

              if (!backFace) {
                dv[v] = height;
                du[u] = width;
              } else {
//                blockType = -blockType;
                du[v] = height;
                dv[u] = width;
              }

              int textureIndex = BlockDB::getTexIndex(block, static_cast<BlockFace>(faceNum));

              int vx = x[0];
              int vy = x[1];
              int vz = x[2];

              // 01 ---- 11
              //
              //
              //
              // 00 ---- 10
              int v00u = du[u] + dv[u];
              int v00v = du[v] + dv[v];
              int v01u = dv[u];
              int v01v = dv[v];
              int v10u = 0;
              int v10v = 0;
              int v11u = du[u];
              int v11v = du[v];

              auto face = static_cast<BlockFace>(faceNum);
              if (face == LEFT) {
                std::swap(v00u, v01v);
                std::swap(v00v, v01u);
                std::swap(v11u, v10v);
                std::swap(v11v, v10u);
                // rotate AO values
                std::swap(currFaceInfo.aoLevels[0], currFaceInfo.aoLevels[1]);
                std::swap(currFaceInfo.aoLevels[1], currFaceInfo.aoLevels[2]);
                std::swap(currFaceInfo.aoLevels[2], currFaceInfo.aoLevels[3]);
              } else if (face == RIGHT) {
                std::swap(v11u, v11v);
                std::swap(v01u, v01v);
                std::swap(v00u, v00v);
                // rotate AO values
                for (int index = 0; index < 2; ++index) {
                  std::swap(currFaceInfo.aoLevels[index], currFaceInfo.aoLevels[index + 2]);
                }
              } else if (face == BACK) {
                for (int index = 0; index < 2; ++index) {
                  std::swap(currFaceInfo.aoLevels[index], currFaceInfo.aoLevels[index + 2]);
                }

              } else if (face == FRONT) {
                std::swap(v00u, v01u);
                std::swap(v00v, v01v);
                std::swap(v11u, v10u);
                std::swap(v11v, v10v);

                // rotate AO values
                std::swap(currFaceInfo.aoLevels[0], currFaceInfo.aoLevels[1]);
                std::swap(currFaceInfo.aoLevels[1], currFaceInfo.aoLevels[2]);
                std::swap(currFaceInfo.aoLevels[2], currFaceInfo.aoLevels[3]);
              } else if (face == BOTTOM) {
                // rotate AO values
                std::swap(currFaceInfo.aoLevels[0], currFaceInfo.aoLevels[1]);
                std::swap(currFaceInfo.aoLevels[1], currFaceInfo.aoLevels[2]);
                std::swap(currFaceInfo.aoLevels[2], currFaceInfo.aoLevels[3]);
              }

              // TODO write func for this???
//              uint32_t vertexData2 =
//                  currFaceInfo.torchLightLevel | ((currFaceInfo.sunlightLevel) << 12) | ((textureIndex & 0xFFF) << 16);
              uint32_t vData2 =
                  createVertexData2(currFaceInfo.torchLightLevel, currFaceInfo.sunlightLevel, textureIndex);
              uint32_t v00Data1 = createVertexData1(vx, vy, vz, currFaceInfo.aoLevels[0], v00u, v00v);
              uint32_t v01Data1 =
                  createVertexData1(vx + du[0], vy + du[1], vz + du[2], currFaceInfo.aoLevels[1], v01u, v01v);
              uint32_t v10Data1 = createVertexData1(vx + du[0] + dv[0],
                                                    vy + du[1] + dv[1],
                                                    vz + du[2] + dv[2],
                                                    currFaceInfo.aoLevels[2],
                                                    v10u,
                                                    v10v);
              uint32_t v11Data1 =
                  createVertexData1(vx + dv[0], vy + dv[1], vz + dv[2], currFaceInfo.aoLevels[3], v11u, v11v);

              bool isTransparent = BlockDB::isTransparent(block);
              auto &vertices = isTransparent ? transparentVertices : opaqueVertices;
              auto &indices = isTransparent ? transparentIndices : opaqueIndices;
              unsigned long baseVertexIndex = vertices.size();
              vertices.push_back({v00Data1, vData2});
              vertices.push_back({v01Data1, vData2});
              vertices.push_back({v10Data1, vData2});
              vertices.push_back({v11Data1, vData2});

              // check whether to flip quad based on AO
              if (!currFaceInfo.flip) {
                //01----------10
                //            /
                //          /
                //        /
                //      /
                //    /
                //  /
                //00----------11

                indices.push_back((baseVertexIndex + 0));
                indices.push_back((baseVertexIndex + 1));
                indices.push_back((baseVertexIndex + 2));
                indices.push_back((baseVertexIndex + 2));
                indices.push_back((baseVertexIndex + 3));
                indices.push_back((baseVertexIndex + 0));
              } else {
                //01----------10
                //  \
              //    \
              //      \
              //        \
              //          \
              //            \
              //00----------11
                indices.push_back(baseVertexIndex + 0);
                indices.push_back(baseVertexIndex + 1);
                indices.push_back(baseVertexIndex + 3);
                indices.push_back(baseVertexIndex + 1);
                indices.push_back(baseVertexIndex + 2);
                indices.push_back(baseVertexIndex + 3);
              }

              // zero out the mask for what we just added
              for (l = 0; l < height; l++) {
                for (k = 0; k < width; k++) {
                  size_t index = counter + k + l * CHUNK_SIZE;
                  infoMask[index] = nullptr;
                  mask[index] = Block::AIR;
                }
              }

              i += width;
              counter += width;

            } else {
              i++;
              counter++;
            }
          }
        }
      }
    }
  }
  delete[] faceInfo;
  static float totalTime = 0;
  count++;
  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() / 1000.0;
  totalTime += duration;
  if (duration > maxTime) {
    maxTime = duration;
  }
  std::cout << duration << " ms, max:  " << maxTime << " ms, avg: " << totalTime / count << std::endl;
}

/**
 * set the face info for this face. For now, includes whether to flip and the four ao values for each vertex
 *
 * @param faceNum (0-5: front, back, right, left, top, bottom)
 * @param blockNeighbors
 */
void FaceInfo::setValues(uint8_t faceNum,
                         const Block (&blockNeighbors)[27],
                         const uint16_t pTorchlightLevel,
                         const uint8_t pSunLightLevel) {
  this->sunlightLevel = pSunLightLevel;
  this->torchLightLevel = pTorchlightLevel;

  bool aoAdjBlocksTransparent[3];
  for (uint8_t vertexNum = 0; vertexNum < 4; vertexNum++) {
    for (uint8_t adjBlockIndex = 0; adjBlockIndex < 3; adjBlockIndex++) {
      Block block = blockNeighbors[LIGHTING_LOOKUP[faceNum][vertexNum][adjBlockIndex]];
      aoAdjBlocksTransparent[adjBlockIndex] = BlockDB::isTransparent(block);
    }
    // source: https://0fps.net/2013/07/03/ambient-occlusion-for-minecraft-like-worlds/
    this->aoLevels[vertexNum] =
        (!aoAdjBlocksTransparent[0] && !aoAdjBlocksTransparent[2] ? 0 : 3 - !aoAdjBlocksTransparent[0]
            - !aoAdjBlocksTransparent[1] - !aoAdjBlocksTransparent[2]);
  }

  this->flip = this->aoLevels[1] + this->aoLevels[3] > this->aoLevels[0] + this->aoLevels[2];
}