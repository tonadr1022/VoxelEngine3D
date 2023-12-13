//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_APPCONSTANTS_HPP
#define VOXEL_ENGINE_APPCONSTANTS_HPP

constexpr int WINDOW_HEIGHT = 900;
constexpr int WINDOW_WIDTH = 1200;
constexpr int CHUNK_WIDTH = 16;
constexpr int CHUNK_HEIGHT = 256;
constexpr int CHUNK_AREA = CHUNK_WIDTH * CHUNK_WIDTH;
constexpr int CHUNKLET_VOLUME = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_WIDTH;
constexpr int CHUNKLET_HEIGHT = CHUNK_WIDTH;
constexpr int CHUNK_VOLUME = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT;
constexpr int NUM_CHUNKLETS = CHUNK_HEIGHT / CHUNKLET_HEIGHT;
constexpr int TEXTURE_ATLAS_WIDTH = 16;
constexpr int MINING_DELAY_MS = 500;
constexpr int PLACING_DELAY_MS = 200;
constexpr glm::vec3 NULL_VECTOR = glm::vec3(-1, -1, -1);

enum class HorizontalDirection {
    LEFT = 0,
    RIGHT,
    FRONT,
    BACK,
};


#endif //VOXEL_ENGINE_APPCONSTANTS_HPP
