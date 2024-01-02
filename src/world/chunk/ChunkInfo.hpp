//
// Created by Tony Adriansen on 1/1/2024.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKINFO_HPP_
#define VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKINFO_HPP_

class ChunkInfo {
 public:
  ChunkInfo() : m_done(false) {
  }
  virtual ~ChunkInfo() = default;

//  std::atomic_bool m_done;
  bool m_done;
};

#endif //VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKINFO_HPP_
