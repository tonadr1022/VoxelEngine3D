//
// Created by Tony Adriansen on 11/16/23.
//

#include "Chunk.h"
#include "ChunkManager.h"
#include "ChunkRenderer.h"


Chunk::Chunk(glm::vec2 location) : location(location), chunkMeshState(ChunkMeshState::UNBUILT),
                                   chunkState(ChunkState::UNDEFINED) {
    for (int chunkZ = 0; chunkZ < CHUNK_HEIGHT; chunkZ += CHUNKLET_HEIGHT) {
        Chunklet chunklet(glm::vec3(location, chunkZ));
        chunklets[chunkZ / CHUNKLET_HEIGHT] = chunklet;
    }
}

void Chunk::buildMesh(Chunk &leftNeighborChunk, Chunk &rightNeighborChunk, Chunk &frontNeighborChunk,
                      Chunk &backNeighborChunk) {
//        ChunkRenderer::destroyGPUResources(*this);
//        unload();
        if (chunkState == ChunkState::UNDEFINED) {
            std::cout << "Chunk is undefined: " << location.x << ", " << location.y << std::endl;
        }
    mesh.construct(*this, leftNeighborChunk, rightNeighborChunk, frontNeighborChunk, backNeighborChunk);
    chunkMeshState = ChunkMeshState::BUILT;
}

void Chunk::load() {

}

void Chunk::unload() {
    mesh.clearData();
    mesh.clearBuffers();
    mesh.isBuffered = false;
    chunkMeshState = ChunkMeshState::UNBUILT;
}

void Chunk::setBlock(int x, int y, int z, Block block) {
    if (block.id != Block::AIR)
        numSolidBlocksInLayers[z]++;
    if (z > getMaxBlockHeightAt(x, y)) {
        setMaxBlockHeightAt(x, y, z);
    }
    int chunkletIndex = z / CHUNKLET_HEIGHT;
    int chunkletZ = z % CHUNKLET_HEIGHT;
    chunklets[chunkletIndex].setBlock(x, y, chunkletZ, block);

}

Block Chunk::getBlock(int x, int y, int z) {
//    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_WIDTH || z < 0 || z >= CHUNK_HEIGHT) {
//        return Block(Block::UNDEFINED);
//    }
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

void Chunk::setIsBlockBuried(int x, int y, int z, bool isBuried) {
    blocksBuried[z * CHUNK_AREA + y * CHUNK_WIDTH + x] = isBuried;
}

void Chunk::markDirty() {
    chunkMeshState = ChunkMeshState::UNBUILT;
    chunkState = ChunkState::CHANGED;
}



