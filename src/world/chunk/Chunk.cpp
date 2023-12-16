//
// Created by Tony Adriansen on 11/16/23.
//

#include "Chunk.hpp"
#include "ChunkManager.hpp"
#include <iostream>

Chunk::Chunk(glm::vec2 location) : location(location), chunkMeshState(ChunkMeshState::UNBUILT),
                                   chunkState(ChunkState::UNGENERATED) {
    m_chunkKey = ChunkManager::getChunkKeyByWorldLocation(location.x, location.y);
    for (int chunkZ = 0; chunkZ < CHUNK_HEIGHT; chunkZ += CHUNKLET_HEIGHT) {
        Chunklet chunklet(glm::vec3(location, chunkZ));
        chunklets[chunkZ / CHUNKLET_HEIGHT] = chunklet;
    }
    maxBlockHeights.fill(0);
    numSolidBlocksInLayers.fill(0);
}

void
Chunk::buildMesh(ChunkManager &chunkManager, Chunk &leftNeighborChunk, Chunk &rightNeighborChunk,
                 Chunk &frontNeighborChunk,
                 Chunk &backNeighborChunk) {
    mesh.construct(chunkManager, *this, leftNeighborChunk, rightNeighborChunk, frontNeighborChunk,
                   backNeighborChunk);
    chunkMeshState = ChunkMeshState::BUILT;
//    chunkState = ChunkState::FULLY_GENERATED;
}

void Chunk::unload() {
    mesh.clearData();
    mesh.clearBuffers();
    chunkState = ChunkState::UNDEFINED;
    chunkMeshState = ChunkMeshState::UNBUILT;
}

bool blockIsAirOrUndefined(Block::ID blockId) {
    return blockId == Block::AIR || blockId == Block::UNDEFINED;
}

void Chunk::setBlock(int x, int y, int z, Block block) {
    Block::ID oldBlockId = getBlock(x, y, z).id;
    Block::ID newBlockId = block.id;
    if (!blockIsAirOrUndefined(oldBlockId) && block.id == Block::AIR) {
        numSolidBlocksInLayers[z]--;
    } else if (blockIsAirOrUndefined(oldBlockId) && newBlockId != Block::AIR) {
        numSolidBlocksInLayers[z]++;
    }

    if (newBlockId != Block::AIR && z > getMaxBlockHeightAt(x, y)) {
        setMaxBlockHeightAt(x, y, z);
    }

//    if (block.id != Block::AIR) {
//        numSolidBlocksInLayers[z]++;
//    }
//
//    if (z > getMaxBlockHeightAt(x, y)) {
//        setMaxBlockHeightAt(x, y, z);
//    }
    int chunkletIndex = z / CHUNKLET_HEIGHT;
    int chunkletZ = z % CHUNKLET_HEIGHT;
    chunklets[chunkletIndex].setBlock(x, y, chunkletZ, block);
}

Block Chunk::getBlock(int x, int y, int z) {
    int chunkletIndex = z / CHUNKLET_HEIGHT;
    int chunkletZ = z % CHUNKLET_HEIGHT;
    Chunklet &chunklet = chunklets[chunkletIndex];
    return chunklet.getBlock(x, y, chunkletZ);
}

glm::vec2 &Chunk::getLocation() {
    return location;
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

bool Chunk::hasNonAirBlockAt(int x, int y, int z) {
    return getBlock(x, y, z).id != Block::AIR;
}


Block Chunk::getBlock(glm::ivec3 &position, ChunkManager &chunkManager) {
    // check if below min height, if so don't add face
    if (position.z < 0) return Block(Block::UNDEFINED);

    // check if adjacent block is vertically out of bounds, if so add face
    if (position.z > CHUNK_HEIGHT) {
        return Block(Block::UNDEFINED);
    }
    // if horizontally out of bounds use world
    if (position.x < 0 || position.x >= CHUNK_WIDTH || position.y < 0 ||
        position.y >= CHUNK_WIDTH) {
        glm::ivec3 worldLocation = glm::ivec3(location, 0) + position;
        return chunkManager.getBlock(worldLocation);
    } else {
        return getBlock(position.x, position.y, position.z);
    }
}

Chunk::~Chunk() = default;