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

class Chunklet;

class Chunk : public std::enable_shared_from_this<Chunk> {
public:
    Chunk() = delete;

    ~Chunk();

    explicit Chunk(glm::ivec2 location);

    static inline int XYZ(int x, int y, int z) {
        return z * CHUNK_AREA + y * CHUNK_WIDTH + x;
    }

    static inline int XYZ(glm::ivec3 pos) {
        return pos.z * CHUNK_AREA + pos.y * CHUNK_WIDTH + pos.x;
    }


void buildMesh(ChunkManager &chunkManager, const Ref<Chunk> &leftNeighborChunk,
               const Ref<Chunk> &rightNeighborChunk, const Ref<Chunk> &frontNeighborChunk,
               const Ref<Chunk> &backNeighborChunk);

void unload();

void setBlock(int x, int y, int z, Block block);

static inline bool outOfBounds(int x, int y, int z) {
    return x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_WIDTH || z < 0 || z >= CHUNK_HEIGHT;
}

inline Block getBlock(int x, int y, int z) const { return m_blocks[XYZ(x, y, z)]; }

inline Block getBlock(const glm::ivec3 &pos) const { return m_blocks[XYZ(pos)]; }

Block getBlock(glm::ivec3 &position, ChunkManager &chunkManager);

bool hasNonAirBlockAt(int x, int y, int z) const;

ChunkMeshState chunkMeshState;
ChunkState chunkState;

glm::ivec2 &getLocation();

inline glm::vec3 location() const { return {m_location.x, m_location.y, 0}; };

ChunkMesh &getMesh();

int getMaxBlockHeightAt(int x, int y);

void setMaxBlockHeightAt(int x, int y, int z);

void markDirty();

std::array<int, CHUNK_HEIGHT> numSolidBlocksInLayers{};

ChunkKey getChunkKey();


std::array<Block, CHUNK_VOLUME> m_blocks;

private:
std::array<int, CHUNK_AREA> maxBlockHeights{};
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
    ChunkLoadInfo(const glm::ivec2 &pos, int seed);

    void process() override;

    void applyTerrain(Chunk *(&chk)[WORLD_HEIGHT]);

private:
    BlockNum m_result[CHUNK_INFO_SIZE * WORLD_HEIGHT]{};
    glm::ivec2 m_position;
    int m_seed;


};


#endif //VOXEL_ENGINE_CHUNK_HPP
