//
// Created by Tony Adriansen on 11/22/23.
//

#ifndef VOXEL_ENGINE_CHUNKKEY_H
#define VOXEL_ENGINE_CHUNKKEY_H

struct ChunkKey {
    int x;
    int y;

    bool operator==(const ChunkKey &other) const {
        return x == other.x && y == other.y;
    }

    bool operator<(const ChunkKey &other) const {
        return x < other.x || (x == other.x && y < other.y);
    }
};

#endif //VOXEL_ENGINE_CHUNKKEY_H

