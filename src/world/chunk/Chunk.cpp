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


ChunkTerrainInfo::ChunkTerrainInfo(glm::ivec2 pos, int seed)
    : m_pos(pos), m_seed(seed) {
}

void ChunkTerrainInfo::generateTerrainData() {
  auto chunkWorldPos = m_pos * CHUNK_SIZE;

  HeightMap heightMap{};
  TerrainGenerator::getHeightMap(chunkWorldPos, m_seed, heightMap);
  m_heightMap = heightMap;

  TreeMap treeMap{};
  TerrainGenerator::getTreeMap(chunkWorldPos, m_seed, treeMap);
  m_treeMap = treeMap;

  TerrainGenerator::generateTerrain(heightMap, m_blocks, m_numBlocksPlaced);
  m_done = true;
}

void ChunkTerrainInfo::applyTerrainDataToChunk(Chunk *(&chunks)[CHUNKS_PER_STACK]) {
  for (int z = 0; z < CHUNKS_PER_STACK; z++) {
    int zOffset0 = z * CHUNK_VOLUME;
    int zOffset1 = zOffset0 + CHUNK_VOLUME;
    std::copy(m_blocks + zOffset0, m_blocks + zOffset1, chunks[z]->m_blocks);
    chunks[z]->chunkState = ChunkState::TERRAIN_GENERATED;
    chunks[z]->m_numNonAirBlocks = m_numBlocksPlaced[z];
  }
}


ChunkLightInfo::ChunkLightInfo(Chunk *chunks[27]) {
  for (int i = 0; i < 27; i++) {
    m_chunks[i] = chunks[i];
  }
}

void ChunkLightInfo::generateLightingData() {
  // populate bloc array
  Block blocks[CHUNK_LIGHT_INFO_SIZE];
  populateBlockArrayForLighting(blocks);

  // torchlight
  std::queue<LightNode> torchlightQueue;
  // populate the queue by iterating through all blocks that have
  // lighting impact on current chunk, checking if any of them are light sources
  glm::ivec3 pos;
  for (int i = 0; i < CHUNK_LIGHT_INFO_SIZE; i++) {
    getPosFromLightArrIndex(pos, i);
    if (BlockDB::isLightSource(blocks[i])) {
      torchlightQueue.push({pos, BlockDB::getLightLevel(blocks[i])});
    }
  }
  propogateTorchLight(torchlightQueue, blocks);

  m_done = true;
}


void ChunkLightInfo::populateBlockArrayForLighting(Block (&blocks)[CHUNK_LIGHT_INFO_SIZE]) {
// iterate through all blocks in the chunk and its neighbors
  // and add them to the block array
  glm::ivec3 pos;
  for (int i = 0; i < CHUNK_LIGHT_INFO_SIZE; i++) {
    getPosFromLightArrIndex(pos, i);
    int offsetX = Utils::chunkNeighborOffset(pos.x);
    int offsetY = Utils::chunkNeighborOffset(pos.y);
    int offsetZ = Utils::chunkNeighborOffset(pos.z);
    int relX = Utils::getRelativeIndex(pos.x);
    int relY = Utils::getRelativeIndex(pos.y);
    int relZ = Utils::getRelativeIndex(pos.z);
    Chunk *chunk = m_chunks[Utils::getNeighborArrayIndex(offsetX, offsetY, offsetZ)];
    if (chunk) {
      blocks[i] = chunk->getBlock(relX, relY, relZ);
    } else {
      blocks[i] = Block::AIR;
    }
  }
}

void ChunkLightInfo::propogateTorchLight(std::queue<LightNode> &torchlightQueue,
                                         Block (&blocks)[CHUNK_LIGHT_INFO_SIZE]) {
  // while the queue is not empty, pop the front element and add it to the light level array
  while (!torchlightQueue.empty()) {
    LightNode node = torchlightQueue.front();
    torchlightQueue.pop();
    const glm::ivec3 &nodeLightLevel = node.lightLevel;
    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace(glm::ivec3(node.pos), faceNum);

      // if outside bounds dont add
//      if (neighborPos[faceNum >> 1] < -14 || neighborPos[faceNum >> 1] >= CHUNK_SIZE + 14) continue;
      if (neighborPos.x < -14 || neighborPos.x >= CHUNK_SIZE + 14 || neighborPos.y < -14 ||
          neighborPos.y >= CHUNK_SIZE + 14 || neighborPos.z < -14 || neighborPos.z >= CHUNK_SIZE + 14) {
        continue;
      }
      int neighborIndex = LIGHT_XYZ(neighborPos.x, neighborPos.y, neighborPos.z);
      Block neighborBlock = blocks[neighborIndex];

      // if light cant pass dont add anything
      if (!BlockDB::canLightPass(neighborBlock)) continue;

      const glm::ivec3 &neighborLightLevel = m_lightLevelArray[neighborIndex];

      if (neighborLightLevel.r < nodeLightLevel.r - 1 ||
          neighborLightLevel.g < nodeLightLevel.g - 1 ||
          neighborLightLevel.b < nodeLightLevel.b - 1) {
        const int newR = std::max(neighborLightLevel.r, nodeLightLevel.r - 1);
        const int newG = std::max(neighborLightLevel.g, nodeLightLevel.g - 1);
        const int newB = std::max(neighborLightLevel.b, nodeLightLevel.b - 1);
        m_lightLevelArray[neighborIndex] = {newR, newG, newB};

        // if there is still light to propogate add to queue
        if (newR > 1 || newG > 1 || newB > 1) {
          torchlightQueue.push({neighborPos, {newR, newG, newB}});
        }
      }
    }
  }
}

void ChunkLightInfo::applyLightDataToChunk(Chunk *chunk) {
  for (int z = 0; z < CHUNK_SIZE; z++) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
      std::copy(m_lightLevelArray + LIGHT_XYZ(0, y, z), m_lightLevelArray + LIGHT_XYZ(CHUNK_SIZE, y, z),
                chunk->m_lightLevels + XYZ(0, y, z));
    }
  }
  chunk->chunkState = ChunkState::FULLY_GENERATED;
}
