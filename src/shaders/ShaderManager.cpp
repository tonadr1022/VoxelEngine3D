//
// Created by Tony Adriansen on 11/16/23.
//

#include "ShaderManager.hpp"

std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderManager::shaders;

void ShaderManager::addShader(std::shared_ptr<Shader> shader,
                              const std::string &name) {
  shaders[name] = std::move(shader);
}

void ShaderManager::useShader(const std::string &name) {
  auto shader = shaders.find(name);
  if (shader != shaders.end()) {
    shader->second->use();
  } else {
    throw std::runtime_error("Shader " + name + " not found");
  }
}

std::shared_ptr<Shader> ShaderManager::getShader(const std::string &name) {
  if (!ShaderManager::shadersCompiled) ShaderManager::compileShaders();
  auto shader = shaders.find(name);
  if (shader != shaders.end()) {
    return shader->second;
  } else {
    throw std::runtime_error("Shader " + name + " not found");
  }
}

void ShaderManager::compileShaders() {
  if (shadersCompiled) return;
  std::shared_ptr<Shader>
      chunkShader = std::make_shared<Shader>("../shaders/ChunkVert.glsl",
                                             "../shaders/ChunkFrag.glsl");
  std::shared_ptr<Shader>
      outlineShader = std::make_shared<Shader>("../shaders/OutlineVert.glsl",
                                               "../shaders/OutlineFrag.glsl",
                                               "../shaders/OutlineGeom.glsl");
  std::shared_ptr<Shader> blockBreakShader = std::make_shared<Shader>(
      "../shaders/BlockBreakVert.glsl", "../shaders/BlockBreakFrag.glsl");
  std::shared_ptr<Shader> crossHairShader = std::make_shared<Shader>(
      "../shaders/CrossHairVert.glsl", "../shaders/CrossHairFrag.glsl");
  addShader(chunkShader, "chunk");
  addShader(outlineShader, "outline");
  addShader(blockBreakShader, "blockBreak");
  addShader(crossHairShader, "crosshair");

  shadersCompiled = true;
}

bool ShaderManager::shadersCompiled = false;
