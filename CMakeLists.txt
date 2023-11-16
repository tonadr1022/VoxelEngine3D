cmake_minimum_required(VERSION 3.26)
project(voxel_engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

find_package(OpenGL REQUIRED)

find_library(GLFW_LIBRARY glfw)

include_directories(src/vendor)

add_executable(voxel_engine
        src/main.cpp
        src/Application.cpp
        src/Application.h)

target_include_directories(voxel_engine PRIVATE dependencies)

target_link_libraries(voxel_engine ${GLFW_LIBRARY} ${OPENGL_gl_LIBRARY})
