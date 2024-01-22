//
// Created by Tony Adriansen on 11/16/23.
//

#include "ShaderManager.hpp"
#include "../EngineConfig.hpp"

std::unordered_map<std::string, std::unique_ptr<Shader>> ShaderManager::m_shaders;

void ShaderManager::addShader(std::unique_ptr<Shader> shader,
                              const std::string &name) {
  m_shaders[name] = std::move(shader);
}


Shader *ShaderManager::getShader(const std::string &name) {
  if (!shadersCompiled) ShaderManager::compileShaders();
  auto it = m_shaders.find(name);
  if (it != m_shaders.end()) {
    return it->second.get();
  } else {
    throw std::runtime_error("Shader " + name + " not found");
  }
}

void ShaderManager::compileShaders() {
  m_shaders.clear();
  std::unique_ptr<Shader> chunkShader = std::make_unique<Shader>(SHADER_PATH(ChunkVert.glsl),
                                                                 SHADER_PATH(ChunkFrag.glsl));
  std::unique_ptr<Shader> outlineShader = std::make_unique<Shader>(SHADER_PATH(OutlineVert.glsl),
                                                                   SHADER_PATH(OutlineFrag.glsl),
                                                                   SHADER_PATH(OutlineGeom.glsl));
  std::unique_ptr<Shader> blockBreakShader = std::make_unique<Shader>(SHADER_PATH(BlockBreakVert.glsl),
                                                                      SHADER_PATH(BlockBreakFrag.glsl));
  std::unique_ptr<Shader> crossHairShader = std::make_unique<Shader>(SHADER_PATH(CrossHairVert.glsl),
                                                                     SHADER_PATH(CrossHairFrag.glsl));
  addShader(std::move(chunkShader), "chunk");
  addShader(std::move(outlineShader), "outline");
  addShader(std::move(blockBreakShader), "blockBreak");
  addShader(std::move(crossHairShader), "crosshair");
  shadersCompiled = true;
}

bool ShaderManager::shadersCompiled = false;
