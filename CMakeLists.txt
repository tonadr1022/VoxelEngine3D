cmake_minimum_required(VERSION 3.7)
project(voxel_engine)

set(CMAKE_CXX_STANDARD 17)

set_property(TARGET PROPERTY CXX_STANDARD 17)

#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

if (UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=core-avx2")
endif ()

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
endif ()

include_directories("dependencies")
include_directories("shaders")
include_directories("resources")

set(SOURCE_FILES
        src/main.cpp
        src/application/Application.cpp
        src/application/Application.hpp
        dependencies/glad/glad.c
        dependencies/imgui/imgui.cpp
        dependencies/imgui/imgui_demo.cpp
        dependencies/imgui/imgui_draw.cpp
        dependencies/imgui/imgui_widgets.cpp
        dependencies/imgui/imgui_impl_glfw.cpp
        dependencies/imgui/imgui_impl_opengl3.cpp
        dependencies/imgui/imgui_tables.cpp
        dependencies/imgui/imgui_widgets.cpp
        dependencies/FastNoiseSIMD/FastNoiseSIMD.cpp
        dependencies/FastNoiseSIMD/FastNoiseSIMD_internal.cpp
        dependencies/FastNoiseSIMD/FastNoiseSIMD_avx2.cpp
        dependencies/FastNoiseSIMD/FastNoiseSIMD_sse2.cpp
        dependencies/FastNoiseSIMD/FastNoiseSIMD_avx512.cpp
        dependencies/FastNoiseSIMD/FastNoiseSIMD_neon.cpp
        dependencies/FastNoiseSIMD/FastNoiseSIMD_sse41.cpp
        src/AppConstants.hpp
        src/camera/Camera.cpp
        src/camera/Camera.hpp
        src/shaders/Shader.cpp
        src/shaders/Shader.hpp
        src/shaders/Shader.cpp
        src/world/block/Block.cpp
        src/world/block/Block.hpp
        src/world/chunk/Chunk.cpp
        src/world/chunk/Chunk.hpp
        src/player/Player.cpp
        src/player/Player.hpp
        src/world/World.cpp
        src/world/World.hpp
        src/world/World.hpp
        src/renderer/Renderer.cpp
        src/renderer/Renderer.hpp
        src/shaders/ShaderManager.cpp
        src/shaders/ShaderManager.hpp
        src/world/chunk/ChunkMesh.cpp
        src/world/chunk/ChunkMesh.hpp
        src/world/generation/TerrainGenerator.cpp
        src/world/generation/TerrainGenerator.hpp
        src/world/chunk/ChunkRenderer.cpp
        src/world/chunk/ChunkRenderer.hpp
        src/resources/ResourceManager.cpp
        src/resources/ResourceManager.hpp
        src/world/block/BlockDB.cpp
        src/world/block/BlockDB.hpp
        src/physics/Ray.hpp
        src/world/events/DigEvent.cpp
        src/world/events/DigEvent.hpp
        src/world/events/IEvent.hpp
        src/utils/Utils.hpp
        src/utils/Utils.cpp
        src/gui/CrossHair.cpp
        src/gui/CrossHair.hpp
        src/renderer/BlockOutlineRenderer.cpp
        src/renderer/BlockOutlineRenderer.hpp
        src/renderer/BlockBreakRenderer.cpp
        src/renderer/BlockBreakRenderer.hpp
        src/player/Inventory.cpp
        src/player/Inventory.hpp
        src/input/Keyboard.cpp
        src/input/Keyboard.hpp
        src/renderer/Renderer2D.cpp
        src/renderer/Renderer2D.hpp
        src/debug_gui/DebugGui.cpp
        src/debug_gui/DebugGui.hpp
        src/gui/Hotbar.cpp
        src/gui/Hotbar.hpp
        src/player/ItemStack.cpp
        src/player/ItemStack.hpp
        src/Config.cpp
        src/Config.hpp
        src/resources/Image.cpp
        src/resources/Image.hpp
        src/EngineConfig.hpp
        src/utils/Timer.cpp
        src/utils/Timer.hpp
        src/game_state/IGameState.hpp
        src/game_state/MenuState.cpp
        src/game_state/MenuState.hpp
        src/application/Window.cpp
        src/application/Window.hpp
        src/input/Mouse.cpp
        src/input/Mouse.hpp
        src/input/KeyPress.hpp
        src/world/save/WorldSave.cpp
        src/world/save/WorldSave.hpp
        src/world/chunk/ChunkMeshBuilder.cpp
        src/world/chunk/ChunkMeshBuilder.hpp
        src/renderer/ViewFrustum.cpp
        src/renderer/ViewFrustum.hpp
        src/math/AABB.hpp
        src/math/Plane.hpp
)

add_executable(voxel_engine ${SOURCE_FILES})

add_subdirectory(dependencies/glfw)
add_subdirectory(dependencies/glm)

find_package(OpenGL REQUIRED)


target_link_libraries(voxel_engine ${OPENGL_LIBRARIES} glfw glm)