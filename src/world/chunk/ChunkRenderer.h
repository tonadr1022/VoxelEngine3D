//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CHUNKRENDERER_H
#define VOXEL_ENGINE_CHUNKRENDERER_H


#include "Chunk.h"
#include "../../camera/Camera.h"
#include "../../shaders/Shader.h"

class ChunkRenderer {
public:
    ChunkRenderer(Camera &camera, unsigned int textureAtlasID);

    ~ChunkRenderer();

    static void createGPUResources(Chunk &chunk);

    static void destroyGPUResources(Chunk &chunk);

    void render(Chunk &chunk);

    void startChunkRender();

private:
    unsigned int textureAtlasID;
    Camera &camera;
    std::shared_ptr<Shader> shader;


};


#endif //VOXEL_ENGINE_CHUNKRENDERER_H
