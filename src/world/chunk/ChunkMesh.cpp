//
// Created by Tony Adriansen on 11/16/23.
//

#include "ChunkMesh.hpp"
#include "Chunk.hpp"
#include "../block/BlockDB.hpp"
#include "../../utils/Timer.hpp"

ChunkMesh::ChunkMesh() : VAO(0), VBO(0), EBO(0) {
}

void ChunkMesh::clearData() {
  vertices.clear();
  indices.clear();
}

void ChunkMesh::clearBuffers() {
  if (VAO != 0) {
    glDeleteVertexArrays(1, &VAO);
    VAO = 0;
  }
  if (VBO != 0) {
    glDeleteBuffers(1, &VBO);
    VBO = 0;
  }
  if (EBO != 0) {
    glDeleteBuffers(1, &EBO);
    EBO = 0;
  }
  isBuffered = false;
}