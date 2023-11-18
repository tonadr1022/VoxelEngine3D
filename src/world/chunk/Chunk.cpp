//
// Created by Tony Adriansen on 11/16/23.
//

#include "Chunk.h"
#include "../world/World.h"


Chunk::Chunk(glm::vec2 location, World &world) : location(location), chunkMeshState(ChunkMeshState::UNBUILT),
                                   chunkState(ChunkState::UNDEFINED), world(world) {

    for (int chunkZ = 0; chunkZ < CHUNK_HEIGHT; chunkZ += CHUNKLET_HEIGHT) {
        Chunklet chunklet(glm::vec3(location, chunkZ));
        chunklets[chunkZ / CHUNKLET_HEIGHT] = chunklet;
    }
}

void Chunk::buildMesh() {
    if (!leftNeighborChunk) {
        leftNeighborChunk = &world.getChunk(location.x, location.y - CHUNK_WIDTH);
    }
    if (!rightNeighborChunk) {
        rightNeighborChunk = &world.getChunk(location.x, location.y + CHUNK_WIDTH);
    }
    if (!frontNeighborChunk) {
        frontNeighborChunk = &world.getChunk(location.x + CHUNK_WIDTH, location.y);
    }
    if (!backNeighborChunk) {
        backNeighborChunk = &world.getChunk(location.x - CHUNK_WIDTH, location.y);
    }
    mesh.construct(*this);
}

void Chunk::load() {

}

void Chunk::unload() {
    mesh.destruct();
}

void Chunk::setBlock(int x, int y, int z, Block block) {
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

//glm::vec3 Chunk::getBlockWorldLocation(int x, int y, int z) const {
//    return {location.x + x, location.y + y, z};
//}

ChunkMesh &Chunk::getMesh() {
    return mesh;
}

Chunk *Chunk::getAdjacentChunk(HorizontalDirection direction) {
    switch (direction) {
        case HorizontalDirection::LEFT:
            return leftNeighborChunk;
        case HorizontalDirection::RIGHT:
            return rightNeighborChunk;
        case HorizontalDirection::FRONT:
            return frontNeighborChunk;
        case HorizontalDirection::BACK:
            return backNeighborChunk;
        default:
            throw std::runtime_error("Invalid direction");
    }
}
