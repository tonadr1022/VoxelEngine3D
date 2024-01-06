//
// Created by Tony Adriansen on 1/5/2024.
//

#include "ChunkLightInfo.hpp"
#include "Chunk.hpp"
#include "../block/BlockDB.hpp"

ChunkLightInfo::ChunkLightInfo(Chunk *chunks[27]) {
  for (int i = 0; i < 27; i++) {
    m_chunks[i] = chunks[i];
  }
}

void ChunkLightInfo::generateLightingData() {
  Chunk *chunk = m_chunks[13];
  // torchlight
  std::queue<LightNode> torchlightQueue;
  // check for any light sources in the chunk and add them to the queue
  for (int blockIndex = 0; blockIndex < CHUNK_VOLUME; blockIndex++) {
    Block block = chunk->getBlockFromIndex(blockIndex);
    if (BlockDB::isLightSource(block)) {
      glm::ivec3 pos = XYZ_FROM_INDEX(blockIndex);
      glm::ivec3 lightLevel = BlockDB::getLightLevel(block);
      torchlightQueue.push({pos, lightLevel});
    }
  }

  propagateTorchLight(torchlightQueue, m_chunks);

  chunk->chunkState = ChunkState::FULLY_GENERATED;
  m_done = true;
}

void ChunkLightInfo::propagateTorchLight(std::queue<LightNode> &torchlightQueue, Chunk *(&chunks)[27]) {
  Chunk *chunk = chunks[13];

  // while the queue is not empty, pop the front element and add it to the light level array
  while (!torchlightQueue.empty()) {
    LightNode node = torchlightQueue.front();
    torchlightQueue.pop();
    // set the value for the light source

    // For each adjacent neighbor for each face of the light source, check whether it has a lower light level
    // for any component. If so, replace the components that are lower in the neighbor with what is propagated
    // from the current light level
    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace(node.pos, faceNum);
      // TODO check for vertical bounds on the neighbor position???
      Block neighborBlock = chunk->getBlockIncludingNeighborsOptimized(neighborPos, chunks);

      // if light cant pass dont add anything
      if (!BlockDB::canLightPass(neighborBlock)) continue;

      const glm::ivec3 neighborLightLevel = chunk->getLightLevelIncludingNeighborsOptimized(neighborPos, chunks);

      if (neighborLightLevel.r < node.lightLevel.r - 1 ||
          neighborLightLevel.g < node.lightLevel.g - 1 ||
          neighborLightLevel.b < node.lightLevel.b - 1) {
        const int newR = std::max(neighborLightLevel.r, node.lightLevel.r - 1);
        const int newG = std::max(neighborLightLevel.g, node.lightLevel.g - 1);
        const int newB = std::max(neighborLightLevel.b, node.lightLevel.b - 1);
        const glm::ivec3 newLightLevel = {newR, newG, newB};
        chunk->setLightLevelIncludingNeighborsOptimized(neighborPos, newLightLevel, chunks);

        // if there is still light to propagate add to queue
        if (newR > 1 || newG > 1 || newB > 1) {
          torchlightQueue.push({neighborPos, newLightLevel});
        }
      }
    }
  }
}

