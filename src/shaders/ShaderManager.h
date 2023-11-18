//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_SHADERMANAGER_H
#define VOXEL_ENGINE_SHADERMANAGER_H

#include "../Config.h"
#include "Shader.h"

class ShaderManager {
public:
    ShaderManager();

    void addShader(const Shader &shader, const std::string &name);

    void useShader(const std::string &name);

private:
    std::unordered_map<std::string, Shader> shaders;


};


#endif //VOXEL_ENGINE_SHADERMANAGER_H
