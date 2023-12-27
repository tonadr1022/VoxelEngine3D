//
// Created by Tony Adriansen on 12/23/23.
//

#include "ChunkAlg.hpp"
#include "Chunk.hpp"
#include "../block/BlockDB.hpp"

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

constexpr std::array<std::array<std::array<glm::ivec3, 3>, 4>, 6>
    lightingAdjacencies =
    {{
         // front face
         {{
              // Bottom Left
              {glm::ivec3(1, 0, -1), glm::ivec3(1, -1, 0),
               glm::ivec3(1, -1, -1)},
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
              {glm::ivec3(-1, 1, 0), glm::ivec3(-1, 0, 1),
               glm::ivec3(-1, 1, 1)},
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
              {glm::ivec3(0, -1, 1), glm::ivec3(1, -1, 0),
               glm::ivec3(1, -1, 1)},
          }},
         // right face
         {{
              // 0, 1, 0
              {glm::ivec3(0, 1, -1), glm::ivec3(-1, 1, 0),
               glm::ivec3(-1, 1, -1)},
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
              {glm::ivec3(0, -1, 1), glm::ivec3(-1, 0, 1),
               glm::ivec3(-1, -1, 1)},
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
              {glm::ivec3(0, 1, -1), glm::ivec3(1, 0, -1),
               glm::ivec3(1, 1, -1)},
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

void ChunkAlg::constructMesh(const Block (&blocks)[82944],
                             std::vector<uint32_t> &vertices,
                             std::vector<unsigned int> &indices) {
  vertices.clear();
  indices.clear();

  int x, y, z, faceIndex, textureX, textureY;
  std::array<int, 20> faceVertices{};

  AdjacentBlockPositions adjacentBlockPositions{};
  // TODO: fix level 255. random blocks appear, so data isn't set properly for it
  for (z = 0; z < CHUNK_HEIGHT - 1; z++) {

    for (x = 0; x < CHUNK_WIDTH; x++) {
      for (y = 0; y < CHUNK_WIDTH; y++) {
        Block block = blocks[MESH_XYZ(x, y, z)];
        if (block == Block::AIR) continue;
        adjacentBlockPositions.update(x, y, z);
        glm::ivec3 blockPos = {x, y, z};
        for (faceIndex = 0; faceIndex < 6; faceIndex++) {
          glm::ivec3
              adjacentBlockPos = adjacentBlockPositions.positions[faceIndex];
          if (adjacentBlockPos.z < 0
              || adjacentBlockPos.z >= CHUNK_HEIGHT)
            continue;

          Block adjacentBlock =
              blocks[MESH_XYZ(adjacentBlockPos.x, adjacentBlockPos.y,
                              adjacentBlockPos.z)];
          if (adjacentBlock != Block::AIR) continue;

          auto face = static_cast<BlockFace>(faceIndex);
          BlockData &blockData = BlockDB::getBlockData(block);
          OcclusionLevels
              occlusionLevels = ChunkAlg::getOcclusionLevels(blockPos,
                                                             face,
                                                             blocks);

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
          auto baseVertexIndex = vertices.size();
          int textureIndex = textureX * TEXTURE_ATLAS_WIDTH + textureY;
          for (int i = 0; i < 20; i += 5) {
            uint32_t encodedValue =
                ((faceVertices[i] + blockPos.x) << 27) |
                    ((faceVertices[i + 1] + blockPos.y) << 22) |
                    ((faceVertices[i + 2] + blockPos.z) << 13) |
                    (occlusionLevels[i / 5] << 10) |
                    (faceVertices[i + 3] << 9) |
                    (faceVertices[i + 4] << 8) |
                    (textureIndex << 0);
            vertices.push_back(encodedValue);
          }

          // check whether to flip quad based on AO
          if (occlusionLevels[0] + occlusionLevels[3] >
              occlusionLevels[1] + occlusionLevels[2]) {
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
      }
    }
  }
}

OcclusionLevels ChunkAlg::getOcclusionLevels(glm::ivec3 &blockPosInChunk,
                                             BlockFace face,
                                             const Block (&blocks)[CHUNK_MESH_INFO_SIZE]) {
  // source: https://0fps.net/2013/07/03/ambient-occlusion-for-minecraft-like-worlds/

  OcclusionLevels occlusionLevels = {3, 3, 3, 3};

  auto &faceLightingAdjacencies = lightingAdjacencies[static_cast<int>(face)];
  for (int faceVertexIndex = 0; faceVertexIndex < 4; faceVertexIndex++) {
    auto &faceLightingAdjacency = faceLightingAdjacencies[faceVertexIndex];
    bool side1IsSolid = false;
    bool side2IsSolid = false;
    bool cornerIsSolid = false;

    glm::ivec3 side1Pos = blockPosInChunk + faceLightingAdjacency[0];
    Block side1Block = blocks[MESH_XYZ(side1Pos.x, side1Pos.y, side1Pos.z)];
    if (side1Block != Block::AIR) {
      side1IsSolid = true;
    }

    glm::ivec3 side2Pos = blockPosInChunk + faceLightingAdjacency[1];
    Block side2Block = blocks[MESH_XYZ(side2Pos.x, side2Pos.y, side2Pos.z)];
    if (side2Block != Block::AIR) {
      side2IsSolid = true;
    }

    glm::ivec3 cornerPos = blockPosInChunk + faceLightingAdjacency[2];
    Block cornerBlock = blocks[MESH_XYZ(cornerPos.x, cornerPos.y, cornerPos.z)];
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