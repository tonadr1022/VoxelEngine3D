//
// Created by Tony Adriansen on 2/10/24.
//

#include "ChunkMeshBuilderLod.hpp"
#include "../block/BlockDB.hpp"
#include "ChunkAlg.hpp"

uint32_t ChunkMeshBuilderLod::createLODVertexData(uint8_t x, uint8_t y, uint8_t z, uint8_t u, uint8_t v) {
  return (x | y << 6 | z << 12 | u << 18 | v << 24);
}
void ChunkMeshBuilderLod::constructMeshGreedyLOD(std::vector<ChunkVertex>& opaqueVertices,
                                                 std::vector<unsigned int>& opaqueIndices,
                                                 std::vector<ChunkVertex>& transparentVertices,
                                                 std::vector<unsigned int>& transparentIndices,
                                                 Chunk* chunk,
                                                 int lodScale) {
  int count = 0;
  auto startTime = std::chrono::high_resolution_clock::now();
  auto maxTime =0;


  auto chunkKey = chunk->m_pos;
  int chunkLen = CHUNK_SIZE / lodScale;
  int chunkMeshInfoLen = chunkLen + 2;
  int chunkMeshInfoArea = chunkMeshInfoLen * chunkMeshInfoLen;
  int chunkMeshInfoVolume = chunkMeshInfoArea * chunkMeshInfoLen;
  Block blockMeshData[chunkMeshInfoVolume];

  for (int i = 0; i < chunkMeshInfoVolume; i++) {
    blockMeshData[i] = Block::AIR;
  }

  auto meshXYZ = [chunkMeshInfoLen, chunkMeshInfoArea](int x, int y, int z) {
    return (x + 1) + (y + 1) * chunkMeshInfoLen + (z + 1) * chunkMeshInfoArea;
  };

  int chunkX, meshDataX, chunkY, meshDataY, chunkZ, meshDataZ;

  if (chunkKey.z > 0) {
    Chunk* negZChunk = chunk->m_neighborChunks[10];
    chunkZ = CHUNK_SIZE - lodScale;
    meshDataZ = -1;
    for (meshDataY = 0, chunkY = 0; meshDataY < chunkLen; meshDataY++, chunkY += lodScale) {
      for (meshDataX = 0, chunkX = 0; meshDataX < chunkLen; meshDataX++, chunkX += lodScale) {
        Block commonBlock = ChunkAlg::getCommonBlockAt(chunkX, chunkY, chunkZ,
                                                       lodScale, lodScale, 1,
                                                       negZChunk->m_blocks, lodScale);
        blockMeshData[meshXYZ(meshDataX, meshDataY, meshDataZ)] = commonBlock;
      }
    }
  }

  if (chunkKey.z < CHUNKS_PER_STACK) {
    Chunk* posZChunk = chunk->m_neighborChunks[16];
    chunkZ = 0;
    meshDataZ = chunkLen;
    for (meshDataY = 0, chunkY = 0; meshDataY < chunkLen; meshDataY++, chunkY += lodScale) {
      for (meshDataX = 0, chunkX = 0; meshDataX < chunkLen; meshDataX++, chunkX += lodScale) {
        Block commonBlock = ChunkAlg::getCommonBlockAt(chunkX, chunkY, chunkZ,
                                                       lodScale, lodScale, 1,
                                                       posZChunk->m_blocks, lodScale);
        blockMeshData[meshXYZ(meshDataX, meshDataY, meshDataZ)] = commonBlock;
      }
    }
  }

  Chunk* negYChunk = chunk->m_neighborChunks[4];
  chunkY = CHUNK_SIZE - lodScale;
  meshDataY = -1;
  for (meshDataZ = 0, chunkZ = 0; meshDataZ < chunkLen; meshDataZ++, chunkZ += lodScale) {
    for (meshDataX = 0, chunkX = 0; meshDataX < chunkLen; meshDataX++, chunkX += lodScale) {
      Block commonBlock = ChunkAlg::getCommonBlockAt(chunkX, chunkY, chunkZ,
                                                     lodScale, 1, lodScale,
                                                     negYChunk->m_blocks, lodScale);
      blockMeshData[meshXYZ(meshDataX, meshDataY, meshDataZ)] = commonBlock;
    }
  }

  Chunk* posYChunk = chunk->m_neighborChunks[22];
  chunkY = 0;
  meshDataY = chunkLen;
  for (meshDataZ = 0, chunkZ = 0; meshDataZ < chunkLen; meshDataZ++, chunkZ += lodScale) {
    for (meshDataX = 0, chunkX = 0; meshDataX < chunkLen; meshDataX++, chunkX += lodScale) {
      Block commonBlock = ChunkAlg::getCommonBlockAt(chunkX, chunkY, chunkZ,
                                                     lodScale, 1, lodScale,
                                                     posYChunk->m_blocks, lodScale);
      blockMeshData[meshXYZ(meshDataX, meshDataY, meshDataZ)] = commonBlock;
    }
  }

  Chunk* negXChunk = chunk->m_neighborChunks[12];
  chunkX = CHUNK_SIZE - lodScale;
  meshDataX = -1;
  for (meshDataZ = 0, chunkZ = 0; meshDataZ < chunkLen; meshDataZ++, chunkZ += lodScale) {
    for (meshDataY = 0, chunkY = 0; meshDataY < chunkLen; meshDataY++, chunkY += lodScale) {
      Block commonBlock = ChunkAlg::getCommonBlockAt(chunkX, chunkY, chunkZ,
                                                     1, lodScale, lodScale,
                                                     negXChunk->m_blocks, lodScale);
      blockMeshData[meshXYZ(meshDataX, meshDataY, meshDataZ)] = commonBlock;
    }
  }

  Chunk* posXChunk = chunk->m_neighborChunks[14];
  chunkX = 0;
  meshDataX = chunkLen;
  for (meshDataZ = 0, chunkZ = 0; meshDataZ < chunkLen; meshDataZ++, chunkZ += lodScale) {
    for (meshDataY = 0, chunkY = 0; meshDataY < chunkLen; meshDataY++, chunkY += lodScale) {
      Block commonBlock = ChunkAlg::getCommonBlockAt(chunkX, chunkY, chunkZ,
                                                     1, lodScale, lodScale,
                                                     posXChunk->m_blocks, lodScale);
      blockMeshData[meshXYZ(meshDataX, meshDataY, meshDataZ)] = commonBlock;
    }
  }

  for (int z = 0; z < chunkLen; z++) {
    for (int y = 0; y < chunkLen; y++) {
      for (int x = 0; x < chunkLen; x++) {
        Block commonBlock = ChunkAlg::getCommonBlockAt(x * lodScale, y * lodScale, z * lodScale,
                                                       lodScale, lodScale, lodScale,
                                                       chunk->m_blocks, lodScale);
        blockMeshData[meshXYZ(x, y, z)] = commonBlock;
      }
    }
  }

//  int z = chunkLen - 1;
//  int y = 0;
//  for (int x = 0; x < chunkLen; x++) {
//    std::cout << BlockDB::getBlockData(blockMeshData[meshXYZ(x, y, z)]).name << " ";
//  }
//  std::cout << '\n';

  int u, v, counter, j, i, k, l, height, width;
  int faceNum;
  int iter1[3]; // start point
  int iter2[3]; // offset
  int du[3];
  int dv[3];
  Block mask[CHUNK_AREA];

  for (bool backFace = true, b = false; b != backFace; backFace = backFace && b, b = !b) {
    for (int axis = 0; axis < 3; axis++) {
      faceNum = ((axis << 1) | (backFace));

      u = (axis + 1) % 3;
      v = (axis + 2) % 3;

      iter1[0] = 0, iter1[1] = 0, iter1[2] = 0, iter1[axis] = -1;
      iter2[0] = 0, iter2[1] = 0, iter2[2] = 0, iter2[axis] = 1;

      // move through chunk from front to back
      while (iter1[axis] < chunkLen) {
        counter = 0;
        for (iter1[v] = 0; iter1[v] < chunkLen; iter1[v]++) {
          for (iter1[u] = 0; iter1[u] < chunkLen; iter1[u]++, counter++) {
            const Block block1 = blockMeshData[meshXYZ(iter1[0], iter1[1], iter1[2])];
            const Block block2 = blockMeshData[meshXYZ(iter1[0] + iter2[0], iter1[1] + iter2[1], iter1[2] + iter2[2])];
            if (!backFace) {
              bool block1OutsideBorder = iter1[axis] < 0;
              if (!block1OutsideBorder && ChunkAlg::shouldShowFace(block1, block2)) {
                mask[counter] = block1;
              } else {
                mask[counter] = Block::AIR;
              }
            } else {
              bool block2OutsideBorder = (chunkLen - 1) <= iter1[axis];
              if (!block2OutsideBorder && ChunkAlg::shouldShowFace(block2, block1)) {
                mask[counter] = block2;
              } else {
                mask[counter] = Block::AIR;
              }
            }
          }
        }

        iter1[axis]++;

        // generate mesh for the mask
        counter = 0;

        for (j = 0; j < chunkLen; j++) {
          for (i = 0; i < chunkLen;) {
            Block block = mask[counter];
            if (block != Block::AIR) { // skip if air
              // compute width
              for (width = 1; block == mask[counter + width] && i + width < chunkLen; width++) {}
              // compute height
              bool done = false;
              for (height = 1; j + height < chunkLen; height++) {
                for (k = 0; k < width; k++) {
                  int index = counter + k + height * chunkLen;
                  if (block != mask[index]) {
                    done = true;
                    break;
                  }
                }
                if (done) break;
              }

              if (width == 15 || height == 15) {
                int sdfsfd = 4;
              }

              iter1[u] = i;
              iter1[v] = j;

              du[0] = 0, du[1] = 0, du[2] = 0;
              dv[0] = 0, dv[1] = 0, dv[2] = 0;

              if (!backFace) {
                dv[v] = height;
                du[u] = width;
              } else {
                du[v] = height;
                dv[u] = width;
              }

              uint32_t textureIndex = BlockDB::getTexIndex(block, static_cast<BlockFace>(faceNum));

              int vx = iter1[0];
              int vy = iter1[1];
              int vz = iter1[2];
              int v00u = (du[u] + dv[u]);
              int v00v = (du[v] + dv[v]);
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
              } else if (face == RIGHT) {
                std::swap(v11u, v11v);
                std::swap(v01u, v01v);
                std::swap(v00u, v00v);
              } else if (face == FRONT) {
                std::swap(v00u, v01u);
                std::swap(v00v, v01v);
                std::swap(v11u, v10u);
                std::swap(v11v, v10v);
              } else if (face == TOP) {
                std::swap(v00u, v01u);
                std::swap(v00v, v01v);
                std::swap(v11u, v10u);
                std::swap(v11v, v10v);
              }

              uint32_t v00Data1 = createLODVertexData((vx) * lodScale,
                                                      (vy) * lodScale,
                                                      (vz) * lodScale,
                                                      (v00u) * lodScale,
                                                      (v00v) * lodScale);
              uint32_t v01Data1 = createLODVertexData((vx + du[0]) * lodScale,
                                                      (vy + du[1]) * lodScale,
                                                      (vz + du[2]) * lodScale,
                                                      (v01u) * lodScale,
                                                      (v01v) * lodScale);
              uint32_t v10Data1 = createLODVertexData((vx + du[0] + dv[0]) * lodScale,
                                                      (vy + du[1] + dv[1]) * lodScale,
                                                      (vz + du[2] + dv[2]) * lodScale,
                                                      (v10u) * lodScale,
                                                      (v10v) * lodScale);
              uint32_t v11Data1 = createLODVertexData((vx + dv[0]) * lodScale,
                                                      (vy + dv[1]) * lodScale,
                                                      (vz + dv[2]) * lodScale,
                                                      (v11u) * lodScale,
                                                      (v11v) * lodScale);
              bool isTransparent = BlockDB::isTransparent(block);
              auto& vertices = isTransparent ? transparentVertices : opaqueVertices;
              auto& indices = isTransparent ? transparentIndices : opaqueIndices;
              unsigned long baseVertexIndex = vertices.size();
              vertices.push_back({v00Data1, textureIndex});
              vertices.push_back({v01Data1, textureIndex});
              vertices.push_back({v10Data1, textureIndex});
              vertices.push_back({v11Data1, textureIndex});

              indices.push_back(baseVertexIndex + 0);
              indices.push_back(baseVertexIndex + 1);
              indices.push_back(baseVertexIndex + 3);
              indices.push_back(baseVertexIndex + 1);
              indices.push_back(baseVertexIndex + 2);
              indices.push_back(baseVertexIndex + 3);


              // zero out the mask for what we just added
              for (l = 0; l < height; l++) {
                for (k = 0; k < width; k++) {
                  size_t index = counter + k + l * chunkLen;
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
    static float totalTime = 0;
  count++;
  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() / 1000.0;
  totalTime += duration;
  if (duration > maxTime) {
    maxTime = duration;
  }
//  std::cout << duration << " ms, max:  " << maxTime << " ms, avg: " << totalTime / count << std::endl;
}