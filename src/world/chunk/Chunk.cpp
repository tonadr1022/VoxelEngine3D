//
// Created by Tony Adriansen on 11/16/23.
//

#include "Chunk.hpp"
#include "ChunkMeshBuilder.hpp"

Chunk::Chunk(glm::ivec3 pos)
    : m_pos(pos),
      m_worldPos(pos * CHUNK_SIZE),
      chunkMeshState(ChunkMeshState::UNBUILT),
      chunkState(ChunkState::UNGENERATED),
      m_boundingBox({m_worldPos, m_worldPos + CHUNK_SIZE}) {}

Chunk::~Chunk() {
  m_transparentMesh.clearBuffers();
  m_transparentMesh.clearData();
  m_opaqueMesh.clearBuffers();
  m_opaqueMesh.clearData();
}

void Chunk::markDirty() {
  chunkMeshState = ChunkMeshState::UNBUILT;
  chunkState = ChunkState::CHANGED;
}

ChunkTerrainInfo::ChunkTerrainInfo(glm::ivec2 pos, int seed)
    : m_pos(pos), m_seed(seed) {
}

void ChunkTerrainInfo::generateTerrainData() {
  auto chunkWorldPos = m_pos * CHUNK_SIZE;
  TerrainGenerator::generateTerrain(chunkWorldPos, m_seed, m_blocks);
  m_done = true;
}

void ChunkTerrainInfo::applyTerrainDataToChunk(Chunk *(&chunks)[CHUNKS_PER_STACK]) {
  for (int z = 0; z < CHUNKS_PER_STACK; z++) {
    int zOffset0 = z * CHUNK_VOLUME;
    int zOffset1 = z * CHUNK_VOLUME + CHUNK_VOLUME;
    std::copy(m_blocks + zOffset0, m_blocks + zOffset1, chunks[z]->m_blocks );
    chunks[z]->chunkState = ChunkState::TERRAIN_GENERATED;
  }
}

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
//  ChunkMeshBuilder meshBuilder(m_chunks);
  ChunkMeshBuilder::constructMesh(m_opaqueVertices, m_opaqueIndices, m_transparentVertices, m_transparentIndices, m_chunks);
  m_done = true;
}

void ChunkMeshInfo::applyMeshDataToMesh(Chunk *chunk) {
  chunk->m_opaqueMesh.vertices = std::move(m_opaqueVertices);
  chunk->m_opaqueMesh.indices = std::move(m_opaqueIndices);
  chunk->m_transparentMesh.vertices = std::move(m_transparentVertices);
  chunk->m_transparentMesh.indices = std::move(m_transparentIndices);
  chunk->chunkMeshState = ChunkMeshState::BUILT;
}

ChunkStructuresInfo::ChunkStructuresInfo(Chunk *(chunks)[27], int seed) : m_terrainGenerator(chunks, seed), m_pos(chunks[13]->m_pos) {

}
void ChunkStructuresInfo::generateStructureData() {
  m_terrainGenerator.generateStructures();
  m_done = true;
}