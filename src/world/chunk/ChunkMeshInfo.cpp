//
// Created by Tony Adriansen on 1/1/2024.
//

#include "ChunkMeshInfo.hpp"
#include "Chunk.hpp"
#include "ChunkMeshBuilder.hpp"


ChunkMeshInfo::ChunkMeshInfo(Chunk *chunks[27]) {
  //  structure of the neighbour arrays
  // y
  // |
  // |  6   15  24
  // |    7   16  25
  // |      8   17  26
  // |
  // |  3   12  21
  // |    4   13  22
  // |      5   14  23
  // \-------------------x
  //  \ 0   9   18
  //   \  1   10  19
  //    \   2   11  20
  //     z
  for (int i = 0; i < 27; i++) {
    m_chunks[i] = chunks[i];
  }
}

void ChunkMeshInfo::generateMeshData() {
  Block blocks[CHUNK_MESH_INFO_SIZE]{};
  populateMeshInfoForMeshing(blocks, m_chunks);
  ChunkMeshBuilder builder(blocks, m_chunks[13]->m_worldPos);
  builder.constructMesh(m_opaqueVertices, m_opaqueIndices, m_transparentVertices, m_transparentIndices);

  m_done = true;
}

void ChunkMeshInfo::applyMeshDataToMesh(Chunk *chunk) {
  chunk->m_opaqueMesh.vertices = std::move(m_opaqueVertices);
  chunk->m_opaqueMesh.indices = std::move(m_opaqueIndices);
  chunk->m_transparentMesh.vertices = std::move(m_transparentVertices);
  chunk->m_transparentMesh.indices = std::move(m_transparentIndices);
  chunk->chunkMeshState = ChunkMeshState::BUILT;
}

void ChunkMeshInfo::populateMeshInfoForMeshing(Block (&result)[CHUNK_MESH_INFO_SIZE], Chunk *(&chunks)[27]) {
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

  int chunkNum, chunkBlockIndex, chunkBlockIndex2, resultBlockIndex, chunkY, chunkZ, meshZ;
#define SET result[resultBlockIndex] = chunks[chunkNum]->m_blocks[chunkBlockIndex];
#define COPY std::copy(chunks[chunkNum]->m_blocks + chunkBlockIndex, chunks[chunkNum]->m_blocks + chunkBlockIndex2, result + resultBlockIndex);

  // Chunks at same z level as middle chunk
  for (chunkZ = 0; chunkZ < CHUNK_SIZE; chunkZ++) {
    chunkNum = 3;
    chunkBlockIndex = XYZ(M1_CHUNK_SIZE, M1_CHUNK_SIZE, chunkZ);
    resultBlockIndex = MESH_XYZ(-1, -1, chunkZ);
    SET;

    chunkNum = 5;
    chunkBlockIndex = XYZ(0, M1_CHUNK_SIZE, chunkZ);
    resultBlockIndex = MESH_XYZ(M1_MESH_SIZE, -1, chunkZ);
    SET;

    chunkNum = 23;
    chunkBlockIndex = XYZ(0, 0, chunkZ);
    resultBlockIndex = MESH_XYZ(M1_MESH_SIZE, M1_MESH_SIZE, chunkZ);
    SET;

    chunkNum = 21;
    chunkBlockIndex = XYZ(M1_CHUNK_SIZE, 0, chunkZ);
    resultBlockIndex = MESH_XYZ(-1, M1_MESH_SIZE, chunkZ);
    SET;

    chunkNum = 4;
    chunkBlockIndex = XYZ(0, M1_CHUNK_SIZE, chunkZ);
    chunkBlockIndex2 = XYZ(CHUNK_SIZE, M1_CHUNK_SIZE, chunkZ); // x not inclusive
    resultBlockIndex = MESH_XYZ(0, -1, chunkZ);
    COPY;

    chunkNum = 22;
    chunkBlockIndex = XYZ(0, 0, chunkZ);
    chunkBlockIndex2 = XYZ(CHUNK_SIZE, 0, chunkZ);
    resultBlockIndex = MESH_XYZ(0, M1_MESH_SIZE, chunkZ); // x not inclusive
    COPY;

    for (chunkY = 0; chunkY < CHUNK_SIZE; chunkY++) {
      chunkNum = 12;
      chunkBlockIndex = XYZ(M1_CHUNK_SIZE, chunkY, chunkZ);
      resultBlockIndex = MESH_XYZ(-1, chunkY, chunkZ);
      SET;

      chunkNum = 14;
      chunkBlockIndex = XYZ(0, chunkY, chunkZ);
      resultBlockIndex = MESH_XYZ(M1_MESH_SIZE, chunkY, chunkZ);
      SET;

      chunkNum = 13;
      chunkBlockIndex = XYZ(0, chunkY, chunkZ);
      chunkBlockIndex2 = XYZ(CHUNK_SIZE, chunkY, chunkZ); // not inclusive
      resultBlockIndex = MESH_XYZ(0, chunkY, chunkZ);
      COPY;
    }
  }

  if (chunkZIndex != 0) {
    chunkZ = M1_CHUNK_SIZE;
    meshZ = -1;

    chunkNum = 0;
    chunkBlockIndex = XYZ(M1_CHUNK_SIZE, M1_CHUNK_SIZE, chunkZ);
    resultBlockIndex = MESH_XYZ(-1, -1, meshZ);
    SET;

    chunkNum = 2;
    chunkBlockIndex = XYZ(0, M1_CHUNK_SIZE, chunkZ);
    resultBlockIndex = MESH_XYZ(M1_MESH_SIZE, -1, meshZ);
    SET;

    chunkNum = 20;
    chunkBlockIndex = XYZ(0, 0, chunkZ);
    resultBlockIndex = MESH_XYZ(M1_MESH_SIZE, M1_MESH_SIZE, meshZ);
    SET;

    chunkNum = 18;
    chunkBlockIndex = XYZ(M1_CHUNK_SIZE, 0, chunkZ);
    resultBlockIndex = MESH_XYZ(-1, M1_MESH_SIZE, meshZ);
    SET;

    chunkNum = 1;
    chunkBlockIndex = XYZ(0, M1_CHUNK_SIZE, chunkZ);
    chunkBlockIndex2 = XYZ(CHUNK_SIZE, M1_CHUNK_SIZE, chunkZ); // x not inclusive
    resultBlockIndex = MESH_XYZ(0, -1, meshZ);
    COPY;

    chunkNum = 19;
    chunkBlockIndex = XYZ(0, 0, chunkZ);
    chunkBlockIndex2 = XYZ(CHUNK_SIZE, 0, chunkZ); // x not inclusive
    resultBlockIndex = MESH_XYZ(0, M1_MESH_SIZE, meshZ);
    COPY;

    for (chunkY = 0; chunkY < CHUNK_SIZE; chunkY++) {
      chunkNum = 9;
      chunkBlockIndex = XYZ(M1_CHUNK_SIZE, chunkY, chunkZ);
      resultBlockIndex = MESH_XYZ(-1, chunkY, meshZ);
      SET;

      chunkNum = 11;
      chunkBlockIndex = XYZ(0, chunkY, chunkZ);
      resultBlockIndex = MESH_XYZ(M1_MESH_SIZE, chunkY, meshZ);
      SET;

      chunkNum = 10;
      chunkBlockIndex = XYZ(0, chunkY, chunkZ);
      chunkBlockIndex2 = XYZ(CHUNK_SIZE, chunkY, chunkZ); // not inclusive
      resultBlockIndex = MESH_XYZ(0, chunkY, meshZ);
      COPY;
    }
  } else {
    // fill bottom mesh layer info with air, top chunk in world
    std::fill(result, result + CHUNK_MESH_INFO_CHUNK_WIDTH * CHUNK_MESH_INFO_CHUNK_WIDTH, Block::AIR);
  }

  // chunks above middle chunk (z offset is 1)
  if (chunkZIndex != CHUNKS_PER_STACK - 1) {
    chunkZ = 0;
    meshZ = M1_MESH_SIZE;

    chunkNum = 6;
    chunkBlockIndex = XYZ(M1_CHUNK_SIZE, M1_CHUNK_SIZE, chunkZ);
    resultBlockIndex = MESH_XYZ(-1, -1, meshZ);
    SET;

    chunkNum = 8;
    chunkBlockIndex = XYZ(0, M1_CHUNK_SIZE, chunkZ);
    resultBlockIndex = MESH_XYZ(M1_MESH_SIZE, -1, meshZ);
    SET;

    chunkNum = 26;
    chunkBlockIndex = XYZ(0, 0, chunkZ);
    resultBlockIndex = MESH_XYZ(M1_MESH_SIZE, M1_MESH_SIZE, meshZ);
    SET;

    chunkNum = 24;
    chunkBlockIndex = XYZ(M1_CHUNK_SIZE, 0, chunkZ);
    resultBlockIndex = MESH_XYZ(-1, M1_MESH_SIZE, meshZ);
    SET;

    chunkNum = 7;
    chunkBlockIndex = XYZ(0, M1_CHUNK_SIZE, chunkZ);
    chunkBlockIndex2 = XYZ(CHUNK_SIZE, M1_CHUNK_SIZE, chunkZ); // x not inclusive
    resultBlockIndex = MESH_XYZ(0, -1, meshZ);
    COPY;

    chunkNum = 25;
    chunkBlockIndex = XYZ(0, 0, chunkZ);
    chunkBlockIndex2 = XYZ(CHUNK_SIZE, 0, chunkZ);
    resultBlockIndex = MESH_XYZ(0, M1_MESH_SIZE, meshZ); // x not inclusive
    COPY;

    for (chunkY = 0; chunkY < CHUNK_SIZE; chunkY++) {
      chunkNum = 15;
      chunkBlockIndex = XYZ(M1_CHUNK_SIZE, chunkY, chunkZ);
      resultBlockIndex = MESH_XYZ(-1, chunkY, meshZ);
      SET;

      chunkNum = 17;
      chunkBlockIndex = XYZ(0, chunkY, chunkZ);
      resultBlockIndex = MESH_XYZ(M1_MESH_SIZE, chunkY, meshZ);
      SET;

      chunkNum = 16;
      chunkBlockIndex = XYZ(0, chunkY, chunkZ);
      chunkBlockIndex2 = XYZ(CHUNK_SIZE, chunkY, chunkZ); // not inclusive
      resultBlockIndex = MESH_XYZ(0, chunkY, meshZ);
      COPY;
    }
  } else {
    // fill top mesh layer info with air, top chunk in world
    std::fill(result + MESH_XYZ(-1, -1, M1_MESH_SIZE), result + CHUNK_MESH_INFO_SIZE, Block::AIR);
  }
}

