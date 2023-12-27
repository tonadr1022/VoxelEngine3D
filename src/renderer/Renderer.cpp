//
// Created by Tony Adriansen on 11/16/23.
//

#include "Renderer.hpp"
#include "../shaders/ShaderManager.hpp"
#include "../resources/ResourceManager.hpp"

Renderer::Renderer() {
  compileShaders();
  loadTextures();
}

void Renderer::renderBlockOutline(Camera &camera, glm::ivec3 blockPosition) {
  m_blockOutlineRenderer.render(blockPosition, camera);
}

void Renderer::renderBlockBreak(Camera &camera,
                                glm::ivec3 blockPosition,
                                int breakStage) {
  m_blockBreakRenderer.render(blockPosition, camera, breakStage);
}

void Renderer::renderCrossHair() const {
  crossHair.render();
}

void Renderer::compileShaders() {
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

  ShaderManager::addShader(chunkShader, "chunk");
  ShaderManager::addShader(outlineShader, "outline");
  ShaderManager::addShader(blockBreakShader, "blockBreak");
  ShaderManager::addShader(crossHairShader, "crosshair");
}

void Renderer::loadTextures() {
  ResourceManager::makeTexture2dArray(
      "../resources/textures/default_pack_512.png",
      "texture_atlas",
      true);
}


