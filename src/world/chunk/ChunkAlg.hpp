//
// Created by Tony Adriansen on 1/6/2024.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKALG_HPP_
#define VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKALG_HPP_
#include "../../EngineConfig.hpp"
#include "Chunk.hpp"

namespace ChunkAlg {
  extern void generateTorchlightData(Chunk *chunk);
 extern void propagateTorchLight(std::queue<LightNode> &torchlightQueue, Chunk *chunk);

};

#endif //VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKALG_HPP_
