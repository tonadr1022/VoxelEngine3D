//
// Created by Tony Adriansen on 11/16/23.
//

#include "ShaderManager.hpp"

std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderManager::shaders;


void ShaderManager::addShader(std::shared_ptr<Shader> shader, const std::string &name) {
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
    auto shader = shaders.find(name);
    if (shader != shaders.end()) {
        return shader->second;
    } else {
        throw std::runtime_error("Shader " + name + " not found");
    }
}
