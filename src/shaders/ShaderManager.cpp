//
// Created by Tony Adriansen on 11/16/23.
//

#include "ShaderManager.h"

ShaderManager::ShaderManager() = default;

void ShaderManager::addShader(const Shader &shader, const std::string &name) {
    shaders.emplace(name, shader);
//    shaders.insert(std::make_pair(name, shader));
}

void ShaderManager::useShader(const std::string &name) {
    auto shader = shaders.find(name);
    if (shader != shaders.end()) {
        shader->second.use();
    } else {
        std::cout << "Shader " << name << " not found" << std::endl;
    }
}
