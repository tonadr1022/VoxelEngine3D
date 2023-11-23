//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CONSTANTS_H
#define VOXEL_ENGINE_CONSTANTS_H

constexpr int WINDOW_HEIGHT = 600;
constexpr int WINDOW_WIDTH = 800;
constexpr int CHUNK_WIDTH = 16;
constexpr int CHUNK_HEIGHT = 256;
constexpr int CHUNK_AREA = CHUNK_WIDTH * CHUNK_WIDTH;
constexpr int CHUNKLET_VOLUME = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_WIDTH;
constexpr int CHUNKLET_HEIGHT = CHUNK_WIDTH;
constexpr int CHUNK_VOLUME = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT;
constexpr int NUM_CHUNKLETS = CHUNK_HEIGHT / CHUNKLET_HEIGHT;
constexpr int TEXTURE_ATLAS_WIDTH = 16;

enum class HorizontalDirection {
    LEFT = 0,
    RIGHT,
    FRONT,
    BACK,
    COUNT
};


#endif //VOXEL_ENGINE_CONSTANTS_H
