cmake_minimum_required(VERSION 3.5.0)
project(voxel_engine VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

find_package(OpenGL REQUIRED)

include_directories(src/vendor)

add_subdirectory(dependencies/GLFW)


add_executable(voxel_engine
        src/main.cpp
        src/Application.cpp
        src/Application.h
        src/glad.c
        src/config.cpp
        src/config.h)

file (COPY resources DESTINATION ${CMAKE_BINARY_DIR})
file (COPY shaders DESTINATION ${CMAKE_BINARY_DIR})

target_include_directories(voxel_engine PRIVATE dependencies)

target_link_libraries(voxel_engine glfw ${OPENGL_gl_LIBRARY})

find_library(GLFW_LIBRARY glfw)

target_link_libraries(voxel_engine ${GLFW_LIBRARY} ${OPENGL_gl_LIBRARY})

