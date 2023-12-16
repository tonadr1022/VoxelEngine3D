//
// Created by Tony Adriansen on 11/22/23.
//

#ifndef VOXEL_ENGINE_CHUNKKEY_HPP
#define VOXEL_ENGINE_CHUNKKEY_HPP

#include <functional>

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

namespace std {
    template <>
    struct hash<ChunkKey> {
        size_t operator()(const ChunkKey& key) const {
            // Combine hashes of individual members
            size_t hashX = hash<int>()(key.x);
            size_t hashY = hash<int>()(key.y);

            // A simple way to combine the hashes
            return hashX ^ (hashY << 1);
        }
    };
}



#endif //VOXEL_ENGINE_CHUNKKEY_HPP

