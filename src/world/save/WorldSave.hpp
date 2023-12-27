//
// Created by Tony Adriansen on 12/22/23.
//

#ifndef VOXEL_ENGINE_WORLDSAVE_HPP
#define VOXEL_ENGINE_WORLDSAVE_HPP

#include "../../EngineConfig.hpp"
#include "../../AppConstants.hpp"
#include "../../world/block/Block.hpp"

class WorldSave {
 public:
  explicit WorldSave(const std::string &path);
  ~WorldSave();

  void saveData();
  void commitChunkData(const glm::ivec2 &pos, Block *data);
 private:
  const std::string &m_path;
  std::unordered_map<glm::ivec2, Block *> m_chunkData;

};

#endif //VOXEL_ENGINE_WORLDSAVE_HPP
