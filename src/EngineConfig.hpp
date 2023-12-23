//
// Created by Tony Adriansen on 12/16/23.
//

#ifndef VOXEL_ENGINE_ENGINECONFIG_HPP
#define VOXEL_ENGINE_ENGINECONFIG_HPP

#include <glad/glad.h>
#include <glm/gtc/noise.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <FastNoiseSIMD/FastNoiseSIMD.h>

#include <utility>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <thread>
#include <unordered_set>
#include <vector>
#include <array>
#include <bitset>
#include <filesystem>
#include <functional>
#include <mutex>
#include <atomic>
#include <future>
#include <chrono>
#include <queue>
#include <condition_variable>
#include <random>
#include <algorithm>
#include <list>


template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T>
using Scope = std::unique_ptr<T>;


#endif //VOXEL_ENGINE_ENGINECONFIG_HPP
