//
// Created by Tony Adriansen on 1/6/2024.
//

#include "ChunkAlg.hpp"
#include "../block/BlockDB.hpp"

void ChunkAlg::generateLightData(ChunkStackArray chunks) {
  for (auto& chunk : chunks) {
    generateTorchlightDataThreaded(chunk);
  }
  generateSunLightData(chunks);
}

void ChunkAlg::generateTorchlightDataThreaded(Chunk* chunk) {
  // torchlight
  std::queue<LightNode> torchlightQueue;
  // check for any light sources in the chunk and add them to the queue
  for (int blockIndex = 0; blockIndex < CHUNK_VOLUME; blockIndex++) {
    Block block = chunk->getBlockFromIndex(blockIndex);
    if (BlockDB::isLightSource(block)) {
      glm::ivec3 pos = XYZ_FROM_INDEX(blockIndex);
      uint16_t lightLevel = BlockDB::getPackedLightLevel(block);
      torchlightQueue.emplace(pos, lightLevel);
      chunk->setTorchLevelIncludingNeighborsOptimized(pos, lightLevel);
    }
  }

  // while the queue is not empty, pop the front element and add it to the light level array
  while (!torchlightQueue.empty()) {
    LightNode node = torchlightQueue.front();
    torchlightQueue.pop();

    // For each adjacent neighbor for each face of the light source, check whether it has a lower light level
    // for any component. If so, replace the components that are lower in the neighbor with what is propagated
    // from the current light level
    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace(node.pos, faceNum);
      Block neighborBlock = chunk->getBlockIncludingNeighborsOptimized(neighborPos);

      // if light cant pass don't add anything
      int neighborLightAttenuation = BlockDB::lightAttenuation(neighborBlock);
      if (neighborLightAttenuation == MAX_LIGHT_ATTENUATION) continue;

      const glm::ivec3 neighborLightLevel = chunk->getTorchLevelIncludingNeighborsOptimized(neighborPos);
      glm::ivec3 unpackedNodeLightLevel = Utils::unpackLightLevel(node.lightLevel);
      if (neighborLightLevel.r < unpackedNodeLightLevel.r - std::max(neighborLightAttenuation, 1) ||
          neighborLightLevel.g < unpackedNodeLightLevel.g - std::max(neighborLightAttenuation, 1) ||
          neighborLightLevel.b < unpackedNodeLightLevel.b - std::max(neighborLightAttenuation, 1)) {
        const int
            newR = std::max(neighborLightLevel.r, unpackedNodeLightLevel.r - std::max(neighborLightAttenuation, 1));
        const int
            newG = std::max(neighborLightLevel.g, unpackedNodeLightLevel.g - std::max(neighborLightAttenuation, 1));
        const int
            newB = std::max(neighborLightLevel.b, unpackedNodeLightLevel.b - std::max(neighborLightAttenuation, 1));
        const uint16_t newLightLevel = Utils::packLightLevel(newR, newG, newB);
        chunk->setTorchLevelIncludingNeighborsOptimized(neighborPos, newLightLevel, true);

        // if there is still light to propagate add to queue
        if (newR > 1 || newG > 1 || newB > 1) {
          torchlightQueue.emplace(neighborPos, newLightLevel);
        }
      }
    }
  }

  chunk->chunkState = ChunkState::FULLY_GENERATED;
}

void ChunkAlg::propagateTorchLight(std::queue<LightNode>& torchlightQueue,
                                   Chunk* chunk,
                                   std::unordered_set<glm::ivec3>& chunkUpdateSet) {

  // while the queue is not empty, pop the front element and add it to the light level array
  while (!torchlightQueue.empty()) {
    LightNode node = torchlightQueue.front();
    torchlightQueue.pop();

    // For each adjacent neighbor for each face of the light source, check whether it has a lower light level
    // for any component. If so, replace the components that are lower in the neighbor with what is propagated
    // from the current light level
    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace(node.pos, faceNum);
      Block neighborBlock = chunk->getBlockIncludingNeighborsOptimized(neighborPos);

      // if light cant pass don't add anything
      int neighborLightAttenuation = BlockDB::lightAttenuation(neighborBlock);
      if (neighborLightAttenuation == MAX_LIGHT_ATTENUATION) continue;

      const glm::ivec3 neighborLightLevel = chunk->getTorchLevelIncludingNeighborsOptimized(neighborPos);
      glm::ivec3 unpackedNodeLightLevel = Utils::unpackLightLevel(node.lightLevel);
      if (neighborLightLevel.r < unpackedNodeLightLevel.r - std::max(neighborLightAttenuation, 1) ||
          neighborLightLevel.g < unpackedNodeLightLevel.g - std::max(neighborLightAttenuation, 1) ||
          neighborLightLevel.b < unpackedNodeLightLevel.b - std::max(neighborLightAttenuation, 1)) {
        const int
            newR = std::max(neighborLightLevel.r, unpackedNodeLightLevel.r - std::max(neighborLightAttenuation, 1));
        const int
            newG = std::max(neighborLightLevel.g, unpackedNodeLightLevel.g - std::max(neighborLightAttenuation, 1));
        const int
            newB = std::max(neighborLightLevel.b, unpackedNodeLightLevel.b - std::max(neighborLightAttenuation, 1));
        const uint16_t newLightLevel = Utils::packLightLevel(newR, newG, newB);
        chunk->setTorchLevelIncludingNeighborsOptimized(neighborPos, newLightLevel, true);

        // if there is still light to propagate add to queue
        if (newR > 1 || newG > 1 || newB > 1) {
          torchlightQueue.emplace(neighborPos, newLightLevel);
        }
      }
    }
  }
}

void ChunkAlg::generateSunLightData(ChunkStackArray& chunks) {
  auto getBlockInChunkStack = [&chunks](int x, int y, int z) -> Block {
    int chunkIndex = z / CHUNK_SIZE;
    return chunks[chunkIndex]->getBlock(x, y, z - chunkIndex * CHUNK_SIZE);
  };

  auto setSunlightInChunkStack = [&chunks](int x, int y, int z, uint8_t lightLevel) {
    int chunkIndex = z / CHUNK_SIZE;
    chunks[chunkIndex]->setSunLightLevel({x, y, z - chunkIndex * CHUNK_SIZE}, lightLevel);
  };

  if (chunks[0]->m_pos.x == 0 && chunks[0]->m_pos.y == 1) {
    int asdfsadf = 2;
  }

  int highestZ = 0;

  // find the highest z value of a block where light cannot pass
  bool done = false;
  for (int chunkIndex = CHUNKS_PER_STACK - 1; chunkIndex >= 0; chunkIndex--) {
    // if no non-air blocks, keep looking
    if (chunks[chunkIndex]->m_numNonAirBlocks == 0) continue;

    // if there's an air block in the chunk, iterate through the chunk until a block is found where light can't pass
    // get the highest z and break out of both loops
    for (int blockIndex = CHUNK_VOLUME - 1; blockIndex >= 0; blockIndex--) {
      if (BlockDB::lightAttenuation(chunks[chunkIndex]->getBlockFromIndex(blockIndex)) != 0) {
        highestZ = chunkIndex * CHUNK_SIZE + (blockIndex / CHUNK_AREA);
        done = true;
        break;
      }
    }
    if (done) break;
  }

  // get the chunk index, chunk, and height in the chunk where the highest block is
  int highestZChunkIndex = highestZ / CHUNK_SIZE;
  Chunk* highestZChunk = chunks[highestZChunkIndex];
  int chunkZ = highestZ - highestZChunkIndex * CHUNK_SIZE;

  // fill everything above the highest block with max light level since nothing blocks it from the sun
  highestZChunk->fillSunlightWithZ(chunkZ + 1, MAX_LIGHT_LEVEL, true);

  // fill everything including the highest z layer and below with 0 since sunlight will be propagated (or not)
  highestZChunk->fillSunlightWithZ(chunkZ, 0, false);


  // fill in chunks above highest block with max light level
  for (int chunkIndex = highestZChunkIndex + 1; chunkIndex < CHUNKS_PER_STACK; chunkIndex++) {
    chunks[chunkIndex]->fillSunlightWithZ(0, MAX_LIGHT_LEVEL, true);
  }

  // fill in chunks below highest z chunk with 0 light level. sunlight may propagate to these chunks
  for (int chunkIndex = 0; chunkIndex < highestZChunkIndex; chunkIndex++) {
    chunks[chunkIndex]->fillSunlightWithZ(0, 0, true);
  }

  std::queue<SunLightNode> sunlightQueue;

//  for (int y = 0; y < CHUNK_SIZE; y++) {
//    for (int x = 0; x < CHUNK_SIZE; x++) {
//      for (int z = highestZ + 1; z >= 0; z--) {
//        Block block = getBlockInChunkStack(x, y, z);
//        if (BlockDB::lightAttenuation(block) == 0) {
//          setSunlightInChunkStack(x, y, z, MAX_LIGHT_LEVEL);
//          sunlightQueue.emplace(x, y, z, MAX_LIGHT_LEVEL);
//        } else {
//          break;
//        }
//      }
//    }
//  }
//  for (int y = 0; y < CHUNK_SIZE; y++) {
//    for (int x = 0; x < CHUNK_SIZE; x++) {
////      for (int z = highestZ + 1; z >= 0; z--) {
//      Block block = getBlockInChunkStack(x, y, highestZ);
//      if (BlockDB::lightAttenuation(block) == 0) {
//        setSunlightInChunkStack(x, y, highestZ, MAX_LIGHT_LEVEL);
//        sunlightQueue.emplace(x, y, highestZ, MAX_LIGHT_LEVEL);
////        } else {
////          break;
//      }
//    }
//  }
  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
//      for (int z = highestZ+1; z >= 0; z--) {
//        Block block = getBlockInChunkStack(x, y, z);
//        if (BlockDB::lightAttenuation(block) == 0) {
          setSunlightInChunkStack(x, y, highestZ+1, MAX_LIGHT_LEVEL);
          sunlightQueue.emplace(x, y, highestZ+1, MAX_LIGHT_LEVEL);
//        } else {
//          break;
//        }
//      }
    }
  }
  propagateSunLight(sunlightQueue, chunks);
}

void ChunkAlg::propagateSunLight(std::queue<SunLightNode>& sunLightQueue, ChunkStackArray& chunks) {
  auto setLightLevelInChunkStackIncludingNeighbors = [&](glm::ivec3 pos, uint8_t lightLevel) {
    int chunkIndex = pos.z / CHUNK_SIZE;
    pos.z -= chunkIndex * CHUNK_SIZE;
    chunks[chunkIndex]->setSunlightIncludingNeighborsOptimized(pos, lightLevel);
  };

  auto getBlockInChunkStackIncludingNeighbors = [&](glm::ivec3 pos) -> Block {
    int chunkIndex = pos.z / CHUNK_SIZE;
    pos.z -= chunkIndex * CHUNK_SIZE;
    return chunks[chunkIndex]->getBlockIncludingNeighborsOptimized(pos);
  };

  auto getSunlightLevelInChunkStackIncludingNeighbors = [&](glm::ivec3 pos) -> uint8_t {
    int chunkIndex = pos.z / CHUNK_SIZE;
    pos.z -= chunkIndex * CHUNK_SIZE;
    return chunks[chunkIndex]->getSunlightLevelIncludingNeighborsOptimized(pos);
  };

  while (!sunLightQueue.empty()) {
    SunLightNode& node = sunLightQueue.front();
    sunLightQueue.pop();
//    if (node.x == 0 && node.y == 31 && node.z == 98 && chunks[0]->m_pos.x == 0 && chunks[0]->m_pos.y == -1) {
//      int sdf = 98;
//    }
    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace({node.x, node.y, node.z}, faceNum);
      if (neighborPos.z < 0 || neighborPos.z >= WORLD_HEIGHT_BLOCKS) continue;
      SunLightNode neighbor = SunLightNode(neighborPos.x, neighborPos.y, neighborPos.z, node.lightLevel);

      Block neighborBlock = getBlockInChunkStackIncludingNeighbors(neighborPos);
      int neighborLightAttenuation = BlockDB::lightAttenuation(neighborBlock);
      if (neighborLightAttenuation == MAX_LIGHT_ATTENUATION) continue;

      int currNeighborSunlightLevel = getSunlightLevelInChunkStackIncludingNeighbors(neighborPos);
      bool copyDown = (faceNum == 5 && neighbor.lightLevel == MAX_LIGHT_LEVEL
          && currNeighborSunlightLevel < neighbor.lightLevel);
      if (copyDown || currNeighborSunlightLevel < neighbor.lightLevel - 1) {
        neighbor.lightLevel = copyDown && neighborLightAttenuation == 0 ? neighbor.lightLevel :
                              std::max(0, neighbor.lightLevel - std::max(neighborLightAttenuation, 1));
        setLightLevelInChunkStackIncludingNeighbors(neighborPos, neighbor.lightLevel);
        sunLightQueue.emplace(neighborPos.x, neighborPos.y, neighborPos.z, neighbor.lightLevel);
      }
    }
  }
}

void ChunkAlg::propagateSunLight(std::queue<SunLightNode>& sunLightQueue,
                                 Chunk* chunk,
                                 std::unordered_set<glm::ivec3>& chunkUpdateSet) {
  std::vector<glm::ivec3> setPositions;

  while (!sunLightQueue.empty()) {
    SunLightNode& node = sunLightQueue.front();
    sunLightQueue.pop();
    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace({node.x, node.y, node.z}, faceNum);

      Block neighborBlock = chunk->getBlockIncludingNeighborsOptimized(neighborPos);
      int neighborLightAttenuation = BlockDB::lightAttenuation(neighborBlock);
      if (neighborLightAttenuation == MAX_LIGHT_ATTENUATION) continue;

      SunLightNode neighbor = SunLightNode(neighborPos.x, neighborPos.y, neighborPos.z, node.lightLevel);

      int currNeighborSunlightLevel = chunk->getSunlightLevelIncludingNeighborsOptimized(neighborPos);

      bool copyDown = (faceNum == 5 && neighbor.lightLevel == MAX_LIGHT_LEVEL
          && currNeighborSunlightLevel < neighbor.lightLevel);
      if (copyDown || currNeighborSunlightLevel < neighbor.lightLevel - 1) {
        neighbor.lightLevel = copyDown && neighborLightAttenuation == 0 ? neighbor.lightLevel :
                              std::max(0, neighbor.lightLevel - std::max(neighborLightAttenuation, 1));
        chunk->setSunlightIncludingNeighborsOptimized(neighborPos, neighbor.lightLevel, true);
        setPositions.push_back(neighborPos);
        sunLightQueue.emplace(neighborPos.x, neighborPos.y, neighborPos.z, neighbor.lightLevel);
      }
    }
  }

  for (const auto& pos : setPositions) {
    World::addRelatedChunks(pos, chunk->m_pos, chunkUpdateSet);
  }
}

void ChunkAlg::unpropagateSunLight(std::queue<SunLightNode>& sunLightPlacementQueue,
                                   std::queue<SunLightNode>& sunlightRemovalQueue,
                                   Chunk* chunk, std::unordered_set<glm::ivec3>& chunkUpdateSet) {
  std::vector<glm::ivec3> setPositions;

  while (!sunlightRemovalQueue.empty()) {
    SunLightNode node = sunlightRemovalQueue.front();
    sunlightRemovalQueue.pop();

    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace({node.x, node.y, node.z}, faceNum);
      const uint8_t neighborLightLevel = chunk->getSunlightLevelIncludingNeighborsOptimized(neighborPos);

      if (neighborLightLevel == 0) continue;

      if (faceNum == 5) {
        chunk->setSunlightIncludingNeighborsOptimized(neighborPos, 0, true);
        setPositions.push_back(neighborPos);
        sunlightRemovalQueue.emplace(neighborPos.x, neighborPos.y, neighborPos.z, MAX_LIGHT_LEVEL);
      } else {
        if (neighborLightLevel >= node.lightLevel) {
          sunLightPlacementQueue.emplace(neighborPos.x, neighborPos.y, neighborPos.z, neighborLightLevel);
        } else {
          chunk->setSunlightIncludingNeighborsOptimized(neighborPos, 0, true);
          setPositions.push_back(neighborPos);
          sunlightRemovalQueue.emplace(neighborPos.x, neighborPos.y, neighborPos.z, neighborLightLevel);
        }
      }
    }
  }

  for (const auto& pos : setPositions) {
    World::addRelatedChunks(pos, chunk->m_pos, chunkUpdateSet);
  }
}

void ChunkAlg::unpropagateTorchLight(std::queue<LightNode>& torchLightPlacementQueue,
                                     std::queue<LightNode>& torchLightRemovalQueue,
                                     Chunk* chunk, std::unordered_set<glm::ivec3>& chunkUpdateSet) {
  std::vector<glm::ivec3> setPositions;

  while (!torchLightRemovalQueue.empty()) {
    LightNode node = torchLightRemovalQueue.front();
    torchLightRemovalQueue.pop();

    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace(node.pos, faceNum);
      const uint16_t neighborLightLevel = chunk->getTorchLevelPackedIncludingNeighborsOptimized(neighborPos);
      if (neighborLightLevel == 0) continue;

      glm::ivec3 unpackedNodeLightLevel = Utils::unpackLightLevel(node.lightLevel);
      glm::ivec3 unpackedNeighborLightLevel = Utils::unpackLightLevel(neighborLightLevel);
      glm::ivec3 newNeighborLightLevel = {0, 0, 0};
      bool place = false;
      if (unpackedNeighborLightLevel.r > 0 && unpackedNodeLightLevel.r > 0
          && unpackedNeighborLightLevel.r >= unpackedNodeLightLevel.r) {
        newNeighborLightLevel.r = unpackedNeighborLightLevel.r;
        place = true;
      }
      if (unpackedNeighborLightLevel.g > 0 && unpackedNodeLightLevel.g > 0
          && unpackedNeighborLightLevel.g >= unpackedNodeLightLevel.g) {
        newNeighborLightLevel.g = unpackedNeighborLightLevel.g;
        place = true;
      }
      if (unpackedNeighborLightLevel.b > 0 && unpackedNodeLightLevel.b > 0
          && unpackedNeighborLightLevel.b >= unpackedNodeLightLevel.b) {
        newNeighborLightLevel.b = unpackedNeighborLightLevel.b;
        place = true;
      }
      if (place) {
        torchLightPlacementQueue.emplace(neighborPos, Utils::packLightLevel(newNeighborLightLevel));
      }
      chunk->setTorchLevelIncludingNeighborsOptimized(neighborPos, 0, true);
      setPositions.push_back(neighborPos);
      int u1 = std::max(unpackedNodeLightLevel.r - 1, 0);
      int u2 = std::max(unpackedNodeLightLevel.g - 1, 0);
      int u3 = std::max(unpackedNodeLightLevel.b - 1, 0);
      if (u1 > 1 || u2 > 1 || u3 > 1) {
        torchLightRemovalQueue.emplace(neighborPos, Utils::packLightLevel(u1, u2, u3));
      }
    }
  }

  for (const auto& pos : setPositions) {
    World::addRelatedChunks(pos, chunk->m_pos, chunkUpdateSet);
  }
}