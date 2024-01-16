//
// Created by Tony Adriansen on 1/6/2024.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKALG_HPP_
#define VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKALG_HPP_
#include "../../EngineConfig.hpp"
#include "Chunk.hpp"
#include "../World.hpp"

namespace ChunkAlg {
  extern void generateLightData(ChunkStackArray chunks);
  extern void generateTorchlightData(Chunk *chunk);

 extern void propagateTorchLight(std::queue<LightNode> &torchlightQueue, Chunk *chunk);
 extern void propagateTorchLightDirect(std::queue<LightNode> &torchlightQueue, World *world);

 extern void unpropagateTorchLightDirect(std::queue<LightNode> &torchLightPlacementQueue, std::queue<LightNode> &torchLightRemovalQueue, World *world);

 extern void generateSunLightData(ChunkStackArray &chunks);
 extern void propagateSunLight(std::queue<SunLightNode> &sunLightQueue, ChunkStackArray chunks);
 extern void propagateSunlightDirect(std::queue<SunLightNodeWorld> &sunlightQueue, World *world);
 extern void propagateSunLight(std::queue<SunLightNode> &sunLightQueue, Chunk *chunk);

 extern void unpropagateSunLightDirect(std::queue<SunLightNodeWorld> &sunLightPlacementQueue, std::queue<SunLightNodeWorld> &sunlightRemovalQueue, World *world);


 extern void unpropagateTorchLight(std::queue<LightNode> &torchLightPlacementQueue,std::queue<LightNode> &torchLightRemovalQueue,
                                   Chunk *chunk);
 extern void unpropagateSunLight(std::queue<SunLightNode> &sunLightPlacementQueue,
                                 std::queue<SunLightNode> &sunlightRemovalQueue,
                                 Chunk *chunk);

};

#endif //VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKALG_HPP_
