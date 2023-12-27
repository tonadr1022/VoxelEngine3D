//
// Created by Tony Adriansen on 11/22/23.
//

#ifndef VOXEL_ENGINE_CHUNKMANAGER_HPP
#define VOXEL_ENGINE_CHUNKMANAGER_HPP


#include "Chunk.hpp"
#include "../../EngineConfig.hpp"


//using ChunkMap = std::unordered_map<ChunkKey, std::shared_ptr<Chunk>>;

class ChunkManager {
public:
    ChunkManager();

//    void remeshChunk(ChunkKey chunkKey);

//    void buildChunkMesh(ChunkKey chunkKey);

//    const Ref<Chunk> &getChunk(ChunkKey chunkKey);

//    Block getBlock(glm::ivec3 position);

//    void setBlock(glm::ivec3 position, Block block);

//    inline bool chunkExists(ChunkKey chunkKey) {
//        return chunkMap.find(chunkKey) != chunkMap.end();
//    }

//    ChunkMap &getChunkMap();


//    inline bool hasChunksToRemesh() {
//        return !m_chunksToRemesh.empty();
//    }

//    void handleChunkUpdates(const Ref<Chunk> &chunk, ChunkKey chunkKey, int chunkX, int chunkY);

//    void remeshChunksToRemesh();

//    void addChunkToRemesh(ChunkKey chunkKey);

private:

//    ChunkMap chunkMap;
//    std::unordered_set<glm::ivec2> m_chunksToRemesh;


};


#endif //VOXEL_ENGINE_CHUNKMANAGER_HPP
