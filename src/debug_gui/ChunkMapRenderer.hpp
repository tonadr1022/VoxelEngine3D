//
// Created by Tony Adriansen on 2/7/24.
//

#ifndef VOXEL_ENGINE_SRC_DEBUG_GUI_CHUNKMAPRENDERER_HPP_
#define VOXEL_ENGINE_SRC_DEBUG_GUI_CHUNKMAPRENDERER_HPP_

#include "../EngineConfig.hpp"

struct QuadVertex {
  float x, y, u, v;
  unsigned int texIndex;
};
class ChunkMapRenderer {
 public:
  inline int flattenIndex(int y, int x) {
    return y*33+x;
  }

  ChunkMapRenderer(int length);
  void draw(float scale) const;
  void genBuffer(std::vector<int> &colors, int loadDistance);

  static constexpr float quadVertices[] = {
      0, 1, 0.0, 1.0, // top left
      1, 1, 1.0, 1.0, // top right
      1, 0, 1.0, 0.0, // bottom right
      0, 0, 0.0, 0.0, // bottom left
  };

  static constexpr unsigned int quadIndices[] = {
      0, 2, 1, 2, 0,3
  };

  std::vector<QuadVertex> vertices;
  std::vector<unsigned int> indices;
  int indices_size;
  int vertices_size;

  bool frameBufferAllocated = false;

 private:
  int m_length;
  unsigned int textureAtlasID;
  GLuint VBO;
  GLuint EBO;
  GLuint VAO;
};

#endif //VOXEL_ENGINE_SRC_DEBUG_GUI_CHUNKMAPRENDERER_HPP_
