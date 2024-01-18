//
// Created by Tony Adriansen on 1/16/2024.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_GENERATION_BIOME_HPP_
#define VOXEL_ENGINE_SRC_WORLD_GENERATION_BIOME_HPP_

#include "../block/Block.hpp"
class Biome {
 public:
  [[nodiscard]] virtual Block getSurfaceBlock() const = 0;
};

class TundraBiome : public Biome {
 public:
  [[nodiscard]] Block getSurfaceBlock() const override {
    return Block::SNOWY_TAIGA_GRASS_BLOCK;
  }
};

class DesertBiome : public Biome {
 public:
  [[nodiscard]] Block getSurfaceBlock() const override {
    return Block::DESERT_GRASS_BLOCK;
  }
};

class JungleBiome : public Biome {
 public:
  [[nodiscard]] Block getSurfaceBlock() const override {
    return Block::JUNGLE_GRASS_BLOCK;
  }
};

class ForestBiome : public Biome {
 public:
  [[nodiscard]] Block getSurfaceBlock() const override {
    return Block::PLAINS_GRASS_BLOCK;
  }
};

class BeachBiome : public Biome {
 public:
  [[nodiscard]] Block getSurfaceBlock() const override {
    return Block::SAND;
  }
};

class OceanBiome : public Biome {
 public:
  [[nodiscard]] Block getSurfaceBlock() const override {
    return Block::SAND;
  }
};

class PlainsBiome : public Biome {
 public:
  [[nodiscard]] Block getSurfaceBlock() const override {
    return Block::PLAINS_GRASS_BLOCK;
  }
};

class SpruceForestBiome : public Biome {
 public:
  [[nodiscard]] Block getSurfaceBlock() const override {
    return Block::SNOWY_TAIGA_GRASS_BLOCK;
  }
};

#endif //VOXEL_ENGINE_SRC_WORLD_GENERATION_BIOME_HPP_
