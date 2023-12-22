//
// Created by Tony Adriansen on 11/16/23.
//

#include "Chunk.hpp"
#include "ChunkManager.hpp"
#include "../../utils/Timer.hpp"

Chunk::Chunk(glm::ivec2 location) : m_location(location), chunkMeshState(ChunkMeshState::UNBUILT),
                                    chunkState(ChunkState::UNGENERATED), m_chunkKey(
                ChunkManager::getChunkKeyByWorldLocation(location.x, location.y)) {
    maxBlockHeights.fill(0);
    numSolidBlocksInLayers.fill(0);
}

void
Chunk::buildMesh(ChunkManager &chunkManager, const Ref<Chunk> &leftNeighborChunk,
                 const Ref<Chunk> &rightNeighborChunk,
                 const Ref<Chunk> &frontNeighborChunk,
                 const Ref<Chunk> &backNeighborChunk) {
    const Ref<Chunk> &sharedThis = std::make_shared<Chunk>(*this);
    mesh.construct(chunkManager, sharedThis, leftNeighborChunk, rightNeighborChunk,
                   frontNeighborChunk,
                   backNeighborChunk);
    chunkMeshState = ChunkMeshState::BUILT;
}

void Chunk::unload() {
    mesh.clearData();
    mesh.clearBuffers();
    chunkState = ChunkState::UNDEFINED;
    chunkMeshState = ChunkMeshState::UNBUILT;
}


void Chunk::setBlock(int x, int y, int z, Block block) {
    Block::ID oldBlockId = getBlock(x, y, z).id;
    Block::ID newBlockId = block.id;
    if (oldBlockId != Block::AIR && block.id == Block::AIR) {
        numSolidBlocksInLayers[z]--;
    } else if (oldBlockId == Block::AIR && newBlockId != Block::AIR) {
        numSolidBlocksInLayers[z]++;
    }
    m_blocks[XYZ(x, y, z)] = block;

    if (m_location.x == 0 && m_location.y == 0 && x == 15 && y == 10 && z == 150) {
        std::cout << XYZ(15, 10, 150) << "In set block chunk" <<  std::endl;
    }
}

glm::ivec2 &Chunk::getLocation() {
    return m_location;
}

ChunkMesh &Chunk::getMesh() {
    return mesh;
}

int Chunk::getMaxBlockHeightAt(int x, int y) {
    return maxBlockHeights[x + y * CHUNK_WIDTH];
}

void Chunk::setMaxBlockHeightAt(int x, int y, int z) {
    maxBlockHeights[x + y * CHUNK_WIDTH] = z;
}

void Chunk::markDirty() {
    chunkMeshState = ChunkMeshState::UNBUILT;
    chunkState = ChunkState::CHANGED;
}

bool Chunk::hasNonAirBlockAt(int x, int y, int z) const {
    return getBlock(x, y, z).id != Block::AIR;
}


Block Chunk::getBlock(glm::ivec3 &position, ChunkManager &chunkManager) {
    if (position.z < 0) return Block(Block::AIR);

    if (position.z >= CHUNK_HEIGHT) {
        return Block(Block::AIR);
    }

    if (position.x < 0 || position.x >= CHUNK_WIDTH || position.y < 0 ||
        position.y >= CHUNK_WIDTH) {
        glm::ivec3 worldLocation = glm::ivec3(m_location, 0) + position;
        return chunkManager.getBlock(worldLocation);
    } else {
        return getBlock(position);
    }
}

ChunkKey Chunk::getChunkKey() {
    return m_chunkKey;
}

Chunk::~Chunk() = default;

ChunkLoadInfo::ChunkLoadInfo(const glm::ivec2 &pos, int seed) : m_position(pos), m_seed(seed) {}

void ChunkLoadInfo::applyTerrain(Chunk *(&chk)[WORLD_HEIGHT]) {

}

void ChunkLoadInfo::process() {
    // make height map
    constexpr int SIZE = 16;
    constexpr int AREA = SIZE * SIZE;
    int heights[AREA];
    int highest = 0;
    auto *heightMap = new float[AREA];  // Allocate memory for the height map

    for (int i = 0; i < AREA; i++) {
        heights[i] = (int)floor(heightMap[i]) * 64 + 100;
        highest = std::max(highest, heights[i]);
    }

    for (int z = 0; z <= highest; z++) {
        int index = 0;
        for (int y = 0; y < SIZE; y++) {
            for (int x = 0; x < SIZE; x++) {
                int index_1d = z * CHUNK_SIZE * CHUNK_SIZE;
                int index_2d = y * SIZE + x;

                index++;
            }
        }
    }


    m_done = true;
    delete []heightMap;}
