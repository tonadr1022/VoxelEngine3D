//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CHUNK_HPP
#define VOXEL_ENGINE_CHUNK_HPP

#include "ChunkMesh.hpp"
#include "../../AppConstants.hpp"
#include "ChunkKey.hpp"
#include "../../EngineConfig.hpp"


enum class ChunkMeshState {
    BUILT = 0,
    UNBUILT,
};

enum class ChunkState {
    TERRAIN_GENERATED = 0,
    FULLY_GENERATED,
    UNGENERATED,
    CHANGED,
    UNDEFINED,
};

class ChunkManager;

static inline int XYZ(int x, int y, int z) {
    return z * CHUNK_AREA + y * CHUNK_WIDTH + x;
}

static inline int XYZ(glm::ivec3 pos) {
    return pos.z * CHUNK_AREA + pos.y * CHUNK_WIDTH + pos.x;
}

static inline int XY(int x, int y) {
    return x + y * CHUNK_WIDTH;
}

static inline int XY(glm::ivec2 &pos) {
    return pos.x + pos.y * CHUNK_WIDTH;
}

static inline int MESH_XYZ(int x, int y, int z) {
    return (z + 1) * CHUNK_MESH_INFO_CHUNK_WIDTH * CHUNK_MESH_INFO_CHUNK_WIDTH +
            (y+1) * CHUNK_MESH_INFO_CHUNK_WIDTH + (x+1);
}

class Chunk {
public:
    Chunk() = delete;

    ~Chunk();

    explicit Chunk(glm::ivec2 location);

    explicit Chunk(ChunkKey chunkKey);

    void unload();

    void setBlock(int x, int y, int z, Block block);

    static inline bool outOfBounds(int x, int y, int z) {
        return x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_WIDTH || z < 0 || z >= CHUNK_HEIGHT;
    }

    inline Block getBlock(int x, int y, int z) const {
        return m_blocks[XYZ(x, y, z)];
    }

    inline Block getBlock(const glm::ivec3 &pos) const { return m_blocks[XYZ(pos)]; }

    Block getBlock(glm::ivec3 &position, ChunkManager &chunkManager);

    bool hasNonAirBlockAt(int x, int y, int z) const;

    ChunkMeshState chunkMeshState;
    ChunkState chunkState;

    glm::ivec2 &getLocation();

    inline glm::vec3 location() const { return {m_location.x, m_location.y, 0}; };

    ChunkMesh &getMesh();

    inline int getMaxTerrainHeightAt(int x, int y) {
        return m_maxTerrainHeights[XY(x, y)];
    };

    inline void setMaxTerrainHeightAt(int x, int y, int z) {
        m_maxTerrainHeights[XY(x, y)] = z;
    };

    void markDirty();

    std::array<int, CHUNK_HEIGHT> numSolidBlocksInLayers{};

    inline ChunkKey chunkKey() const { return m_chunkKey; };

    std::array<int, CHUNK_AREA> m_maxTerrainHeights{};

    Block m_blocks[CHUNK_VOLUME]{};

private:

    ChunkMesh mesh;
    glm::ivec2 m_location;
    ChunkKey m_chunkKey;
};


class ChunkInfo {
public:
    std::atomic_bool m_done;

    virtual void process() = 0;

    ChunkInfo() : m_done(false) {}
};

class ChunkLoadInfo : public ChunkInfo {
public:
    ChunkLoadInfo(ChunkKey chunkKey, int seed);

    void process() override;

    void applyTerrain(const Ref<Chunk> &chunk);

private:
    Block m_blocks[CHUNK_VOLUME]{};
    int m_seed;
    ChunkKey m_chunkKey;
};

class ChunkMeshInfo : public ChunkInfo {
public:
    explicit ChunkMeshInfo(Ref<Chunk> chunk[9]);

    void process() override;

    void applyMesh(const Ref<Chunk> &chunk);

private:
    Block m_blocks[CHUNK_MESH_INFO_SIZE]{};
    std::vector<uint32_t> m_vertices;
    std::vector<unsigned int> m_indices;
    ChunkKey m_chunkKey;


};


#endif //VOXEL_ENGINE_CHUNK_HPP
