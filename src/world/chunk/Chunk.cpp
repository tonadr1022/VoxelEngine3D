//
// Created by Tony Adriansen on 11/16/23.
//

#include "Chunk.hpp"
#include "ChunkMeshBuilder.hpp"
#include "../block/BlockDB.hpp"

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

void Chunk::setLightLevelIncludingNeighborsOptimized(const glm::ivec3 pos,
                                                     const glm::ivec3 lightLevel,
                                                     Chunk *(&chunks)[27]) {
  if (isPosOutOfChunkBounds(pos)) {
    int neighborArrayIndex = Utils::getChunkNeighborArrayIndexFromOutOfBoundsPos(pos);
    Chunk *neighborChunk = chunks[neighborArrayIndex];
    if (neighborChunk) {
      glm::ivec3 localPos = Utils::outOfBoundsPosToLocalPos(pos);
      neighborChunk->setLightLevel(localPos, lightLevel);
    }
  } else {
    setLightLevel(pos, lightLevel);
  }
}

void Chunk::setBlockIncludingNeighborsOptimized(glm::ivec3 pos, Block block, Chunk *(&chunks)[27]) {
  if (isPosOutOfChunkBounds(pos)) {
    int neighborArrayIndex = Utils::getChunkNeighborArrayIndexFromOutOfBoundsPos(pos);
    Chunk *neighborChunk = chunks[neighborArrayIndex];
    if (neighborChunk) {
      glm::ivec3 localPos = Utils::outOfBoundsPosToLocalPos(pos);
      neighborChunk->setBlock(localPos, block);
    }
  } else {
    setBlock(pos, block);
  }
}

Block Chunk::getBlockIncludingNeighborsOptimized(glm::ivec3 pos, Chunk *(&chunks)[27]) const {
  if (isPosOutOfChunkBounds(pos)) {
    int neighborArrayIndex = Utils::getChunkNeighborArrayIndexFromOutOfBoundsPos(pos);
    const Chunk *neighborChunk = chunks[neighborArrayIndex];
    if (neighborChunk) {
      glm::ivec3 localPos = Utils::outOfBoundsPosToLocalPos(pos);
      return neighborChunk->getBlock(localPos);
    }
  } else {
    return getBlock(pos);
  }
}

glm::ivec3 Chunk::getLightLevelIncludingNeighborsOptimized(glm::ivec3 pos, Chunk *(&chunks)[27]) const {
  if (isPosOutOfChunkBounds(pos)) {
    int neighborArrayIndex = Utils::getChunkNeighborArrayIndexFromOutOfBoundsPos(pos);
    const Chunk *neighborChunk = chunks[neighborArrayIndex];
    if (neighborChunk) {
      glm::ivec3 localPos = Utils::outOfBoundsPosToLocalPos(pos);
      return neighborChunk->getLightLevel(localPos);
    }
  } else {
    return getLightLevel(pos);
  }
}

