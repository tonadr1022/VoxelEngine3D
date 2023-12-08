//
// Created by Tony Adriansen on 11/16/23.
//

#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>


GLint Shader::makeModule(const std::string &filepath, GLint module_type) {
    // open file and read contents into string stream (one big string)
    std::ifstream file;
    std::stringstream bufferedLines;
    std::string line;

    file.open(filepath);

    if (!file.is_open()) {
        std::cout << "ERROR::SHADER::FILE_NOT_FOUND\n";
        return -1;
    }

    while (std::getline(file, line)) {
        bufferedLines << line << '\n';
    }

    std::string shaderSource = bufferedLines.str();
    const char *shaderSrc = shaderSource.c_str();
    bufferedLines.str("");
    file.close();

    GLint shaderModule = glCreateShader(module_type);
    glShaderSource(shaderModule, 1, &shaderSrc, nullptr);
    glCompileShader(shaderModule);

    int success;
    glGetShaderiv(shaderModule, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderModule, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::"
                  << (module_type == GL_VERTEX_SHADER ?
                      "VERTEX" : module_type == GL_FRAGMENT_SHADER ?
                                 "FRAGMENT" : "GEOMETRY")
                  << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        return -1;
    }
    return shaderModule;


}

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath) {
    std::vector<GLint> shaderModules;

    GLint vertexShaderModule = makeModule(vertexPath, GL_VERTEX_SHADER);
    shaderModules.push_back(vertexShaderModule);

    GLint fragmentShaderModule = makeModule(fragmentPath, GL_FRAGMENT_SHADER);
    shaderModules.push_back(fragmentShaderModule);

    if (!geometryPath.empty()) {
        GLint geometryShaderModule = makeModule(geometryPath, GL_GEOMETRY_SHADER);
        shaderModules.push_back(geometryShaderModule);
    }


    shaderProgram = glCreateProgram();
    for (GLint shaderModule: shaderModules) {
        glAttachShader(shaderProgram, shaderModule);
    }
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        char errorLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, errorLog);
        std::cout << "Shader Program linking error:\n"
                  << errorLog << std::endl;
    }

    for (GLint shaderModule: shaderModules) {
        glDeleteShader(shaderModule);
    }
}

void Shader::use() const {
    glUseProgram(shaderProgram);
}

void Shader::unbind() const {
    glDeleteProgram(shaderProgram);
}


void Shader::setBool(const std::string &name, bool value) const {
    GLint location = getUniformLocation(name);
    glUniform1i(location, (int) value);
}

void Shader::setInt(const std::string &name, int value) const {
    GLint location = getUniformLocation(name);
    glUniform1i(location, value);
}

void Shader::setFloat(const std::string &name, float value) const {
    GLint location = getUniformLocation(name);
    glUniform1f(location, value);
}

void Shader::setMat4(const std::string &name, glm::mat4 &matrix) const {
    GLint location = getUniformLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setVec3(const std::string &name, glm::vec3 &vector) const {
    GLint location = getUniformLocation(name);
    glUniform3f(location, vector[0], vector[1], vector[2]);
}

GLint Shader::getUniformLocation(const std::string &name) const {
    if (uniformLocationCache.find(name) != uniformLocationCache.end())
        return uniformLocationCache[name];

    GLint location = glGetUniformLocation(shaderProgram, name.c_str());
    uniformLocationCache[name] = location;
    return location;
}