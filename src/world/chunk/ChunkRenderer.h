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
    ChunkRenderer(Camera &camera, Shader &shader, unsigned int textureAtlasID);

    ~ChunkRenderer();

    void createGPUResources(Chunk &chunk);

    void destroyGPUResources(Chunk &chunk);

    void render(Chunk &chunk);

private:
    unsigned int textureAtlasID;
    Camera &camera;
    Shader &shader;


};


#endif //VOXEL_ENGINE_CHUNKRENDERER_H
