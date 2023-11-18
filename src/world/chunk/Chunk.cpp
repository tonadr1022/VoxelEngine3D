//
// Created by Tony Adriansen on 11/16/23.
//

#include "Chunk.h"


Chunk::Chunk(glm::vec2 location) : location(location), chunkMeshState(ChunkMeshState::UNBUILT),
                                   chunkState(ChunkState::UNDEFINED) {

    for (int chunkZ = 0; chunkZ < CHUNK_HEIGHT; chunkZ += CHUNKLET_HEIGHT) {
        Chunklet chunklet(glm::vec3(location, chunkZ), *this);
        chunklets[chunkZ / CHUNKLET_HEIGHT] = chunklet;
    }
}

void Chunk::buildMesh() {
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
    return chunklets[chunkletIndex].getBlock(x, y, chunkletZ);
}

glm::vec2& Chunk::getLocation() {
    return location;
}

//glm::vec3 Chunk::getBlockWorldLocation(int x, int y, int z) const {
//    return {location.x + x, location.y + y, z};
//}

ChunkMesh& Chunk::getMesh() {
    return mesh;
}
