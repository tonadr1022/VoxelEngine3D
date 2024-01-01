//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_APPCONSTANTS_HPP
#define VOXEL_ENGINE_APPCONSTANTS_HPP

#include "EngineConfig.hpp"

constexpr int DEFAULT_WINDOW_HEIGHT = 900;
constexpr int DEFAULT_WINDOW_WIDTH = 1200;
constexpr int CHUNK_SIZE = 32;
constexpr int CHUNKS_PER_STACK = 8;
constexpr int CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
constexpr int CHUNK_VOLUME = CHUNK_SIZE * CHUNK_AREA;
constexpr int WORLD_HEIGHT_BLOCKS = CHUNK_SIZE * CHUNKS_PER_STACK;
constexpr int TEXTURE_ATLAS_WIDTH = 16;
constexpr int MINING_DELAY_MS = 500;
constexpr int PLACING_DELAY_MS = 200;
constexpr glm::vec3 NULL_VECTOR = glm::vec3(-1, -1, -1);

constexpr int CHUNK_MESH_INFO_CHUNK_WIDTH = CHUNK_SIZE + 2;

enum class HorizontalDirection {
  LEFT = 0,
  RIGHT,
  FRONT,
  BACK,
};

#endif //VOXEL_ENGINE_APPCONSTANTS_HPP
