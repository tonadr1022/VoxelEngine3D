//
// Created by Tony Adriansen on 1/1/2024.
//

#include "ChunkMeshInfo.hpp"
#include "Chunk.hpp"
#include "ChunkMeshBuilder.hpp"

ChunkMeshInfo::ChunkMeshInfo(Chunk *chunk) : m_chunk(chunk) {
}

void ChunkMeshInfo::generateMeshData() {
  Block blocks[CHUNK_MESH_INFO_SIZE]{};
  uint16_t torchLightLevels[CHUNK_MESH_INFO_SIZE]{};
  uint8_t sunlightLevels[CHUNK_MESH_INFO_SIZE]{};
  populateMeshInfoForMeshing(blocks, torchLightLevels, sunlightLevels, m_chunk->m_neighborChunks);
  ChunkMeshBuilder builder(blocks, torchLightLevels, sunlightLevels, m_chunk->m_worldPos);
//  builder.constructMesh(m_opaqueVertices, m_opaqueIndices, m_transparentVertices, m_transparentIndices);
  builder.constructMeshGreedy(m_opaqueVertices, m_opaqueIndices, m_transparentVertices, m_transparentIndices);
  m_done = true;
}

void ChunkMeshInfo::applyMeshDataToMesh() {
  m_chunk->m_opaqueMesh.vertices = std::move(m_opaqueVertices);
  m_chunk->m_opaqueMesh.indices = std::move(m_opaqueIndices);
  m_chunk->m_transparentMesh.vertices = std::move(m_transparentVertices);
  m_chunk->m_transparentMesh.indices = std::move(m_transparentIndices);
  m_chunk->chunkMeshState = ChunkMeshState::BUILT;
}

void ChunkMeshInfo::populateMeshInfoForMeshing(Block (&blockResult)[CHUNK_MESH_INFO_SIZE],
                                               uint16_t (&torchResult)[CHUNK_MESH_INFO_SIZE],
                                               uint8_t (&sunlightResult)[CHUNK_MESH_INFO_SIZE],
                                               Chunk *(&chunks)[27]) {
  const int chunkZIndex = chunks[13]->m_pos.z;
  constexpr int M1_CHUNK_SIZE = CHUNK_SIZE - 1;
  constexpr int M1_MESH_SIZE = CHUNK_SIZE;

  //  structure of the neighbour arrays
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

  int chunkNum, chunkInfoIndex, chunkInfoIndex2, resultInfoIndex, chunkY, chunkZ, meshZ;
#define SET blockResult[resultInfoIndex] = chunks[chunkNum]->m_blocks[chunkInfoIndex]; \
if (chunks[chunkNum]->m_torchLightLevelsPtr) {torchResult[resultInfoIndex] = chunks[chunkNum]->m_torchLightLevelsPtr.get()[chunkInfoIndex]; }\
if (chunks[chunkNum]->m_sunlightLevelsPtr) {sunlightResult[resultInfoIndex] = chunks[chunkNum]->m_sunlightLevelsPtr.get()[chunkInfoIndex]; }
#define COPY std::copy(chunks[chunkNum]->m_blocks + chunkInfoIndex, chunks[chunkNum]->m_blocks + chunkInfoIndex2, blockResult + resultInfoIndex); \
if (chunks[chunkNum]->m_torchLightLevelsPtr) { std::copy(chunks[chunkNum]->m_torchLightLevelsPtr.get() + chunkInfoIndex,\
chunks[chunkNum]->m_torchLightLevelsPtr.get() + chunkInfoIndex2, torchResult + resultInfoIndex); }\
if (chunks[chunkNum]->m_sunlightLevelsPtr) { std::copy(chunks[chunkNum]->m_sunlightLevelsPtr.get() + chunkInfoIndex,\
chunks[chunkNum]->m_sunlightLevelsPtr.get() + chunkInfoIndex2, sunlightResult + resultInfoIndex); }
  // Chunks at same z level as middle chunk
  for (chunkZ = 0; chunkZ < CHUNK_SIZE; chunkZ++) {
    chunkNum = 3;
    chunkInfoIndex = XYZ(M1_CHUNK_SIZE, M1_CHUNK_SIZE, chunkZ);
    resultInfoIndex = MESH_XYZ(-1, -1, chunkZ);
    SET;

    chunkNum = 5;
    chunkInfoIndex = XYZ(0, M1_CHUNK_SIZE, chunkZ);
    resultInfoIndex = MESH_XYZ(M1_MESH_SIZE, -1, chunkZ);
    SET;

    chunkNum = 23;
    chunkInfoIndex = XYZ(0, 0, chunkZ);
    resultInfoIndex = MESH_XYZ(M1_MESH_SIZE, M1_MESH_SIZE, chunkZ);
    SET;

    chunkNum = 21;
    chunkInfoIndex = XYZ(M1_CHUNK_SIZE, 0, chunkZ);
    resultInfoIndex = MESH_XYZ(-1, M1_MESH_SIZE, chunkZ);
    SET;

    chunkNum = 4;
    chunkInfoIndex = XYZ(0, M1_CHUNK_SIZE, chunkZ);
    chunkInfoIndex2 = XYZ(M1_CHUNK_SIZE, M1_CHUNK_SIZE, chunkZ) + 1; // x not inclusive
    resultInfoIndex = MESH_XYZ(0, -1, chunkZ);
    COPY;

    chunkNum = 22;
    chunkInfoIndex = XYZ(0, 0, chunkZ);
    chunkInfoIndex2 = XYZ(M1_CHUNK_SIZE, 0, chunkZ) + 1;
    resultInfoIndex = MESH_XYZ(0, M1_MESH_SIZE, chunkZ); // x not inclusive
    COPY;

    for (chunkY = 0; chunkY < CHUNK_SIZE; chunkY++) {
      chunkNum = 12;
      chunkInfoIndex = XYZ(M1_CHUNK_SIZE, chunkY, chunkZ);
      resultInfoIndex = MESH_XYZ(-1, chunkY, chunkZ);
      SET;

      chunkNum = 14;
      chunkInfoIndex = XYZ(0, chunkY, chunkZ);
      resultInfoIndex = MESH_XYZ(M1_MESH_SIZE, chunkY, chunkZ);
      SET;

      chunkNum = 13;
      chunkInfoIndex = XYZ(0, chunkY, chunkZ);
      chunkInfoIndex2 = XYZ(M1_CHUNK_SIZE, chunkY, chunkZ) + 1; // not inclusive
      resultInfoIndex = MESH_XYZ(0, chunkY, chunkZ);
      COPY;
    }
  }

  if (chunkZIndex != 0) {
    chunkZ = M1_CHUNK_SIZE;
    meshZ = -1;

    chunkNum = 0;
    chunkInfoIndex = XYZ(M1_CHUNK_SIZE, M1_CHUNK_SIZE, chunkZ);
    resultInfoIndex = MESH_XYZ(-1, -1, meshZ);
    SET;

    chunkNum = 2;
    chunkInfoIndex = XYZ(0, M1_CHUNK_SIZE, chunkZ);
    resultInfoIndex = MESH_XYZ(M1_MESH_SIZE, -1, meshZ);
    SET;

    chunkNum = 20;
    chunkInfoIndex = XYZ(0, 0, chunkZ);
    resultInfoIndex = MESH_XYZ(M1_MESH_SIZE, M1_MESH_SIZE, meshZ);
    SET;

    chunkNum = 18;
    chunkInfoIndex = XYZ(M1_CHUNK_SIZE, 0, chunkZ);
    resultInfoIndex = MESH_XYZ(-1, M1_MESH_SIZE, meshZ);
    SET;

    chunkNum = 1;
    chunkInfoIndex = XYZ(0, M1_CHUNK_SIZE, chunkZ);
    chunkInfoIndex2 = XYZ(M1_CHUNK_SIZE, M1_CHUNK_SIZE, chunkZ) + 1; // x not inclusive
    resultInfoIndex = MESH_XYZ(0, -1, meshZ);
    COPY;

    chunkNum = 19;
    chunkInfoIndex = XYZ(0, 0, chunkZ);
    chunkInfoIndex2 = XYZ(M1_CHUNK_SIZE, 0, chunkZ) + 1; // x not inclusive
    resultInfoIndex = MESH_XYZ(0, M1_MESH_SIZE, meshZ);
    COPY;

    for (chunkY = 0; chunkY < CHUNK_SIZE; chunkY++) {
      chunkNum = 9;
      chunkInfoIndex = XYZ(M1_CHUNK_SIZE, chunkY, chunkZ);
      resultInfoIndex = MESH_XYZ(-1, chunkY, meshZ);
      SET;

      chunkNum = 11;
      chunkInfoIndex = XYZ(0, chunkY, chunkZ);
      resultInfoIndex = MESH_XYZ(M1_MESH_SIZE, chunkY, meshZ);
      SET;

      chunkNum = 10;
      chunkInfoIndex = XYZ(0, chunkY, chunkZ);
      chunkInfoIndex2 = XYZ(M1_CHUNK_SIZE, chunkY, chunkZ) + 1; // not inclusive
      resultInfoIndex = MESH_XYZ(0, chunkY, meshZ);
      COPY;
    }
  } else {
    // fill bottom mesh layer info with air, top chunk in world
    std::fill(blockResult, blockResult + CHUNK_MESH_INFO_CHUNK_WIDTH * CHUNK_MESH_INFO_CHUNK_WIDTH, Block::AIR);
  }

  // chunks above middle chunk (z offset is 1)
  if (chunkZIndex != CHUNKS_PER_STACK - 1) {
    chunkZ = 0;
    meshZ = M1_MESH_SIZE;

    chunkNum = 6;
    chunkInfoIndex = XYZ(M1_CHUNK_SIZE, M1_CHUNK_SIZE, chunkZ);
    resultInfoIndex = MESH_XYZ(-1, -1, meshZ);
    SET;

    chunkNum = 8;
    chunkInfoIndex = XYZ(0, M1_CHUNK_SIZE, chunkZ);
    resultInfoIndex = MESH_XYZ(M1_MESH_SIZE, -1, meshZ);
    SET;

    chunkNum = 26;
    chunkInfoIndex = XYZ(0, 0, chunkZ);
    resultInfoIndex = MESH_XYZ(M1_MESH_SIZE, M1_MESH_SIZE, meshZ);
    SET;

    chunkNum = 24;
    chunkInfoIndex = XYZ(M1_CHUNK_SIZE, 0, chunkZ);
    resultInfoIndex = MESH_XYZ(-1, M1_MESH_SIZE, meshZ);
    SET;

    chunkNum = 7;
    chunkInfoIndex = XYZ(0, M1_CHUNK_SIZE, chunkZ);
    chunkInfoIndex2 = XYZ(M1_CHUNK_SIZE, M1_CHUNK_SIZE, chunkZ) + 1; // x not inclusive
    resultInfoIndex = MESH_XYZ(0, -1, meshZ);
    COPY;

    chunkNum = 25;
    chunkInfoIndex = XYZ(0, 0, chunkZ);
    chunkInfoIndex2 = XYZ(M1_CHUNK_SIZE, 0, chunkZ) + 1;
    resultInfoIndex = MESH_XYZ(0, M1_MESH_SIZE, meshZ); // x not inclusive
    COPY;

    for (chunkY = 0; chunkY < CHUNK_SIZE; chunkY++) {
      chunkNum = 15;
      chunkInfoIndex = XYZ(M1_CHUNK_SIZE, chunkY, chunkZ);
      resultInfoIndex = MESH_XYZ(-1, chunkY, meshZ);
      SET;

      chunkNum = 17;
      chunkInfoIndex = XYZ(0, chunkY, chunkZ);
      resultInfoIndex = MESH_XYZ(M1_MESH_SIZE, chunkY, meshZ);
      SET;

      chunkNum = 16;
      chunkInfoIndex = XYZ(0, chunkY, chunkZ);
      chunkInfoIndex2 = XYZ(M1_CHUNK_SIZE, chunkY, chunkZ) + 1; // not inclusive
      resultInfoIndex = MESH_XYZ(0, chunkY, meshZ);
      COPY;
    }
  } else {
    // fill top mesh layer info with air, top chunk in world
    std::fill(blockResult + MESH_XYZ(-1, -1, M1_MESH_SIZE), blockResult + CHUNK_MESH_INFO_SIZE, Block::AIR);
  }
}

