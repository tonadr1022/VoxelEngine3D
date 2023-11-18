//
// Created by Tony Adriansen on 11/16/23.
//

#include "ChunkRenderer.h"

ChunkRenderer::ChunkRenderer(Camera &camera, Shader &shader, unsigned int textureAtlasID) : camera(camera),
                                                                                            shader(shader),
                                                                                            textureAtlasID(
                                                                                                    textureAtlasID) {
}


void ChunkRenderer::render(Chunk &chunk) {
    if (chunk.chunkMeshState == ChunkMeshState::UNBUILT) {
        chunk.buildMesh();
        createGPUResources(chunk);
    }
    if (chunk.chunkMeshState == ChunkMeshState::FAILED) {
        std::cout << "Chunk mesh failed to build, cant render" << std::endl;
        return;
    }
    // model matrix for rendering chunk
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(chunk.getLocation(), 0.0f));

    shader.use();
    shader.setInt("u_Texture", 0);
    shader.setMat4("u_Model", model);
    shader.setMat4("u_Projection", camera.getProjectionMatrix());
    shader.setMat4("u_View", camera.getViewMatrix());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureAtlasID);
    glBindVertexArray(chunk.getMesh().VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.getMesh().EBO);

    glDrawElements(GL_TRIANGLES, static_cast<GLint>(chunk.getMesh().indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ChunkRenderer::createGPUResources(Chunk &chunk) {
    ChunkMesh &mesh = chunk.getMesh();

    glGenVertexArrays(1, &mesh.VAO);
    glBindVertexArray(mesh.VAO);

    glGenBuffers(1, &mesh.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), &mesh.vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &mesh.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

    // set vertex attributes pointers
    // position vector
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, (void *) nullptr);
    glEnableVertexAttribArray(0);
    // texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 32, (void *) 12);
    glEnableVertexAttribArray(1);
    // normal vector
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 32, (void *) 20);
    glEnableVertexAttribArray(2);
}

void ChunkRenderer::destroyGPUResources(Chunk &chunk) {
    ChunkMesh &mesh = chunk.getMesh();
    glDeleteVertexArrays(1, &mesh.VAO);
    glDeleteBuffers(1, &mesh.VBO);
    glDeleteBuffers(1, &mesh.EBO);
}


ChunkRenderer::~ChunkRenderer() = default;
