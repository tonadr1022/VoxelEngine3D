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