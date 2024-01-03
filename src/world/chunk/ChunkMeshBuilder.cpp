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
    0, 0, 0, 0, 0,
    0, 0, 1, 0, 1,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 1,
};

constexpr std::array<int, 20> rightFace{
    0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1,
};

constexpr std::array<int, 20> leftFace{
    0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1,
};

constexpr std::array<int, 20> topFace{
    0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1,
};

constexpr std::array<int, 20> bottomFace{
    0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1,
};

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


ChunkMeshBuilder::ChunkMeshBuilder(Block (&blocks)[CHUNK_MESH_INFO_SIZE], const glm::ivec3 &chunkPos) : m_blocks(
    blocks), m_chunkPos(chunkPos) {
}

void ChunkMeshBuilder::constructMesh(std::vector<ChunkVertex> &opaqueVertices,
                                     std::vector<unsigned int> &opaqueIndices,
                                     std::vector<ChunkVertex> &transparentVertices,
                                     std::vector<unsigned int> &transparentIndices) {
  auto opaqueVertices_ = new uint32_t[20000];
  auto transparentVertices_ = new uint32_t[20000];
  auto opaqueIndices_ = new unsigned int[20000];
  auto transparentIndices_ = new unsigned int[20000];

  int opaqueVerticesIndex = 0;
  int transparentVerticesIndex = 0;
  int opaqueIndicesIndex = 0;
  int transparentIndicesIndex = 0;

  int chunkBaseZ = m_chunkPos.z * CHUNK_SIZE;
  int x, y, z, faceIndex, textureX, textureY;
  std::array<int, 20> faceVertices{};

  for (int chunkBlockIndex = 0; chunkBlockIndex < CHUNK_VOLUME; chunkBlockIndex++) {
    x = chunkBlockIndex & 31;
    y = (chunkBlockIndex >> 5) & 31;
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
//  opaqueVertices = std::vector<uint32_t>(opaqueVertices_, opaqueVertices_ + opaqueVerticesIndex);
  opaqueIndices = std::vector<unsigned int>(opaqueIndices_, opaqueIndices_ + opaqueIndicesIndex);
//  transparentVertices = std::vector<uint32_t>(transparentVertices_, transparentVertices_ + transparentVerticesIndex);
  transparentIndices = std::vector<unsigned int>(transparentIndices_, transparentIndices_ + transparentIndicesIndex);

  delete[] opaqueVertices_;
  delete[] transparentVertices_;
  delete[] opaqueIndices_;
  delete[] transparentIndices_;
}
void ChunkMeshBuilder::constructMeshGreedy(std::vector<ChunkVertex> &opaqueVertices,
                                           std::vector<unsigned int> &opaqueIndices,
                                           std::vector<ChunkVertex> &transparentVertices,
                                           std::vector<unsigned int> &transparentIndices) {

  // get face data
//  FaceInfo faceInfo[CHUNK_VOLUME][6] = {}; // need to initialize in case skip faces, which will be left as 0
  auto faceInfo = new FaceInfo[CHUNK_VOLUME][6]; // need to initialize in case skip faces, which will be left as 0

  Block blockNeighbors[27];
  for (int chunkBlockIndex = 0; chunkBlockIndex < CHUNK_VOLUME; chunkBlockIndex++) {

    // derive block position from the index
    int blockPos[3];
    blockPos[0] = chunkBlockIndex & 31; // X (right most 5 bits in the index value
    blockPos[1] = (chunkBlockIndex >> 5) & 31; // Y (middle 5 bits in the index value)
    blockPos[2] = chunkBlockIndex >> 10; // Z (left most 5 bits in the index value)

    // get block, if its air, none of the faces should be added
    Block block = m_blocks[MESH_XYZ(blockPos[0], blockPos[1], blockPos[2])];
    if (block == Block::AIR) continue;

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

      if (!shouldShowFace(block, neighborBlock)) continue;

      // fill neighbors for block if haven't. only do this on first iteration of face loop after at least one face should be shown.
      if (!neighborsInitializedForCurrentBlock) {
        int neighborIndex = 0;
        int iterator[3];

        // see neighbor array offsets in World.hpp
        for (iterator[1] = blockPos[1] - 1; iterator[1] <= blockPos[1] + 1; ++iterator[1]) {
          for (iterator[2] = blockPos[2] - 1; iterator[2] <= blockPos[2] + 1; ++iterator[2]) {
            for (iterator[0] = blockPos[0] - 1; iterator[0] <= blockPos[0] + 1; ++iterator[0]) {
              blockNeighbors[neighborIndex++] = m_blocks[MESH_XYZ(iterator[0], iterator[1], iterator[2])];
            }
          }
        }
        neighborsInitializedForCurrentBlock = true;
      }

      // with neighbors and knowledge that this face will be added to the mesh, add its face data to the array
      faceInfo[chunkBlockIndex][faceNum].setValues(faceNum, blockNeighbors);
    }
  }

  int u, v, counter, j, i, k, l, height, width;
  int faceNum;
  int x[3]; // start point
  int q[3]; // offset
  int du[3];
  int dv[3];

  for (int axis = 0; axis < 3; axis++) {
    FaceInfo *infoMask[CHUNK_AREA]; // use pointers since data already exists in the faceInfo array above
    int mask[CHUNK_AREA];
    u = (axis + 1) % 3;
    v = (axis + 2) % 3;

    x[0] = 0, x[1] = 0, x[2] = 0, x[axis] = -1;
    q[0] = 0, q[1] = 0, q[2] = 0, q[axis] = 1;

//    if (axis == 0) {
//      faceNum = isBackFace ? 1 : 0;
//    } else if (axis == 1) {
//      faceNum = isBackFace ? 3 : 2;
//    } else {
//      faceNum = isBackFace ? 5 : 4;
//    }

    // move through chunk from front to back
    while (x[axis] < CHUNK_SIZE) {
      counter = 0;
      for (x[v] = 0; x[v] < CHUNK_SIZE; x[v]++) {
        for (x[u] = 0; x[u] < CHUNK_SIZE; x[u]++, counter++) {
          const Block block1 = m_blocks[MESH_XYZ(x[0], x[1], x[2])];
          const Block block2 = m_blocks[MESH_XYZ(x[0] + q[0], x[1] + q[1], x[2] + q[2])];

          bool block1ChunkBorder = x[axis] < 0;
          bool block2ChunkBorder = CHUNK_SIZE - 1 <= x[axis];

          if (!block1ChunkBorder && shouldShowFace(block1, block2)) {
            mask[counter] = static_cast<int>(block1);
            infoMask[counter] = &faceInfo[XYZ(x[0], x[1], x[2])][axis << 1];
          } else if (!block2ChunkBorder && shouldShowFace(block2, block1)) {
            mask[counter] = -static_cast<int>(block2);
            infoMask[counter] = &faceInfo[XYZ(x[0] + q[0], x[1] + q[1], x[2] + q[2])][(axis << 1) | 1];
          } else {
            infoMask[counter] = nullptr;
            mask[counter] = 0;
          }
        }
      }

      x[axis]++;

      // generate mesh for the mask
      counter = 0;

      for (j = 0; j < CHUNK_SIZE; j++) {
        for (i = 0; i < CHUNK_SIZE;) {
          int blockType = mask[counter];
          if (blockType) { // skip if air or zeroed
            const FaceInfo &currFaceInfo = *infoMask[counter];
            // compute width
            for (width = 1; blockType == mask[counter + width]
                && infoMask[counter + width]
                && currFaceInfo == *infoMask[counter + width]
                && i + width < CHUNK_SIZE;
                 width++) {}

            // compute height
            bool done = false;
            for (height = 1; j + height < CHUNK_SIZE; height++) {
              for (k = 0; k < width; k++) {
                int index = counter + k + height * CHUNK_SIZE;
                if (blockType != mask[index] || !infoMask[index] || currFaceInfo != *infoMask[index]) {
                  done = true;
                  break;
                }
              }
              if (done) break;
            }

            x[u] = i;
            x[v] = j;

            du[0] = 0;
            du[1] = 0;
            du[2] = 0;
            dv[0] = 0;
            dv[1] = 0;
            dv[2] = 0;

            faceNum = ((axis << 1) | (blockType <= 0));

            if (blockType > 0) {
              dv[v] = height;
              du[u] = width;
            } else {
              blockType = -blockType;
              du[v] = height;
              dv[u] = width;
            }
            // add the quad
            int textureIndex = BlockDB::getTextureIndex(static_cast<Block>(blockType), static_cast<BlockFace>(faceNum));

            int vx = m_chunkPos.x * CHUNK_SIZE + x[0];
            int vy = m_chunkPos.y * CHUNK_SIZE + x[1];
            int vz = m_chunkPos.z * CHUNK_SIZE + x[2];

            int v00u = du[u] + dv[u];
            int v00v = du[v] + dv[v];
            int v01u = dv[u];
            int v01v = dv[v];
            int v10u = 0;
            int v10v = 0;
            int v11u = du[u];
            int v11v = du[v];

            auto face = static_cast<BlockFace>(faceNum);
//            if (face == BlockFace::LEFT) {
//              std::swap(v00u, v01v);
//              std::swap(v00v, v01u);
//              std::swap(v11u, v10v);
//              std::swap(v11v, v10u);
//            } else if (face == BlockFace::RIGHT) {
//              std::swap(v11u, v11v);
//              std::swap(v01u, v01v);
//              std::swap(v00u, v00v);
//            } else if (face == BlockFace::FRONT) {
//              std::swap(v00u, v01u);
//              std::swap(v00v, v01v);
//              std::swap(v11u, v10u);
//              std::swap(v11v, v10v);
//            }

            // TODO write func for this???
            ChunkVertex v00 = {static_cast<uint32_t>((vx & 0x3F) | ((vy & 0x3F) << 6) | ((vz & 0x3F) << 12) |
                ((currFaceInfo.aoLevels[0] & 0x3) << 18) |
                ((v00u & 0x1) << 20) |
                ((v00v & 0x1) << 21) |
                ((textureIndex & 0xFF) << 22)), static_cast<float>(v00u), static_cast<float>(v00v),
                               static_cast<float>(textureIndex), (float) (vx), (float) (vy), (float) (vz)};

            ChunkVertex v01 =
                {static_cast<uint32_t>((vx + du[0] & 0x3F) | ((vy + du[1] & 0x3F) << 6) | ((vz + du[2] & 0x3F) << 12) |
                    ((currFaceInfo.aoLevels[1] & 0x3) << 18) | ((v01u & 0x1) << 20) | ((v01v & 0x1) << 21)
                    | ((textureIndex & 0xFF) << 22)), static_cast<float>(v01u), static_cast<float>(v01v),
                 static_cast<float>(textureIndex), (float) (vx + du[0]), (float) (vy + du[1]), (float) (vz + du[2])};

            ChunkVertex v10 = {
                static_cast<uint32_t>((vx + du[0] + dv[0] & 0x3F) | ((vy + du[1] + dv[1] & 0x3F) << 6)
                    | ((vz + du[2] + dv[2] & 0x3F) << 12) |
                    ((currFaceInfo.aoLevels[2] & 0x3) << 18) | ((v10u & 0x1) << 20) | ((v10v & 0x1) << 21)
                    | ((textureIndex & 0xFF) << 22)), static_cast<float>(v10u), static_cast<float>(v10v),
                static_cast<float>(textureIndex), (float) (vx + du[0] + dv[0]), (float) (vy + du[1] + dv[1]),
                (float) (vz + du[2] + dv[2])};

            ChunkVertex v11 = {
                static_cast<uint32_t>((vx + dv[0] & 0x3F) | ((vy + dv[1] & 0x3F) << 6) | ((vz + dv[2] & 0x3F) << 12) |
                    ((currFaceInfo.aoLevels[3] & 0x3) << 18) | ((v11u & 0x1) << 20) | ((v11v & 0x1) << 21)
                    | ((textureIndex & 0xFF) << 22)), static_cast<float>(v11u), static_cast<float>(v11v),
                static_cast<float>(textureIndex), (float) (vx + dv[0]), (float) (vy + dv[1]), (float) (vz + dv[2])};

            bool isTransparent = BlockDB::isTransparent(static_cast<Block>(blockType));
            auto &vertices = isTransparent ? transparentVertices : opaqueVertices;
            auto &indices = isTransparent ? transparentIndices : opaqueIndices;
            unsigned long baseVertexIndex = vertices.size();
            vertices.push_back(v00);
            vertices.push_back(v01);
            vertices.push_back(v10);
            vertices.push_back(v11);

            // check whether to flip quad based on AO
            if (!currFaceInfo.flip) {
//              indices.push_back(baseVertexIndex + 2);
//              indices.push_back(baseVertexIndex);
//              indices.push_back(baseVertexIndex + 3);
//              indices.push_back(baseVertexIndex + 3);
//              indices.push_back(baseVertexIndex);
//              indices.push_back(baseVertexIndex + 1);
              indices.push_back(baseVertexIndex + 3);
              indices.push_back(baseVertexIndex + 1);
              indices.push_back(baseVertexIndex + 2);
              indices.push_back(baseVertexIndex + 0);
              indices.push_back(baseVertexIndex + 1);
              indices.push_back(baseVertexIndex + 3);
            } else {
              //11-----------
              //
              //
              //
              //
              //
              //
              //00----------10
              indices.push_back(baseVertexIndex + 3);
              indices.push_back(baseVertexIndex + 1);
              indices.push_back(baseVertexIndex + 2);
              indices.push_back(baseVertexIndex + 0);
              indices.push_back(baseVertexIndex + 1);
              indices.push_back(baseVertexIndex + 3);
              indices.push_back(baseVertexIndex);
              indices.push_back(baseVertexIndex);
              indices.push_back(baseVertexIndex);

            }



            // zero out the mask for what we just added
            for (l = 0; l < height; l++) {
              for (k = 0; k < width; k++) {
                size_t index = counter + k + l * CHUNK_SIZE;
                infoMask[index] = nullptr;
                mask[index] = 0;
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
//  delete[] faceInfo;
  std::cout << opaqueVertices.size() << std::endl;
}

OcclusionLevels ChunkMeshBuilder::getOcclusionLevels(const glm::ivec3 &blockPosInChunk,
                                                     BlockFace face) {

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

/**
 * set the face info for this face. For now, includes whether to flip and the four ao values for each vertex
 *
 * @param faceNum (0-5: front, back, right, left, top, bottom)
 * @param blockNeighbors
 */
void FaceInfo::setValues(uint8_t faceNum, const Block (&blockNeighbors)[27]) {
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
  this->blockId = static_cast<uint8_t>(blockNeighbors[13]);
  this->flip = this->aoLevels[0] + this->aoLevels[3] > this->aoLevels[1] + this->aoLevels[2];
}
