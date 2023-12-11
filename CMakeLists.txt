cmake_minimum_required(VERSION 3.7)
project(voxel_engine)

set(CMAKE_CXX_STANDARD 17)

set_property(TARGET PROPERTY CXX_STANDARD 17)

#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include_directories(src/vendor)

set(SOURCE_FILES
                src/main.cpp
                src/Application.cpp
                src/Application.h
                src/glad.c
        src/vendor/imgui/imgui.cpp
        src/vendor/imgui/imgui_demo.cpp
        src/vendor/imgui/imgui_draw.cpp
        src/vendor/imgui/imgui_widgets.cpp
        src/vendor/imgui/imgui_impl_glfw.cpp
        src/vendor/imgui/imgui_impl_opengl3.cpp
        src/vendor/imgui/imgui_tables.cpp
        src/vendor/imgui/imgui_widgets.cpp
        src/Constants.h
        src/camera/Camera.cpp
        src/camera/Camera.h
        src/shaders/Shader.cpp
        src/shaders/Shader.h
        src/shaders/Shader.cpp
        src/world/block/Block.cpp
        src/world/block/Block.h
        src/world/chunk/Chunk.cpp
        src/world/chunk/Chunk.h
        src/player/Player.cpp
        src/player/Player.h
        src/world/World.cpp
        src/world/World.h
        src/world/World.h
        src/renderer/Renderer.cpp
        src/renderer/Renderer.h
        src/shaders/ShaderManager.cpp
        src/shaders/ShaderManager.h
        src/world/chunklet/Chunklet.cpp
        src/world/chunklet/Chunklet.h
        src/world/chunk/ChunkMesh.cpp
        src/world/chunk/ChunkMesh.h
        src/world/generation/TerrainGenerator.cpp
        src/world/generation/TerrainGenerator.h
        src/world/chunk/ChunkRenderer.cpp
        src/world/chunk/ChunkRenderer.h
        src/resources/ResourceManager.cpp
        src/resources/ResourceManager.h
        src/world/chunk/ChunkManager.cpp
        src/world/chunk/ChunkManager.h
        src/world/chunk/ChunkKey.h
        src/world/block/BlockDB.cpp
        src/world/block/BlockDB.h
        src/physics/Ray.h
        src/world/events/DigEvent.cpp
        src/world/events/DigEvent.h
        src/world/events/IEvent.h
        src/utils/Utils.h
        src/utils/Utils.cpp
        src/gui/CrossHair.cpp
        src/gui/CrossHair.h
        src/renderer/BlockOutlineRenderer.cpp
        src/renderer/BlockOutlineRenderer.h
        src/renderer/BlockBreakRenderer.cpp
        src/renderer/BlockBreakRenderer.h
        src/player/Inventory.cpp
        src/player/Inventory.h
        src/input/Keyboard.cpp
        src/input/Keyboard.h
        src/renderer/Renderer2D.cpp
        src/renderer/Renderer2D.h
        src/renderer/GuiRenderer.cpp
        src/renderer/GuiRenderer.h
        src/debug_gui/DebugGui.cpp
        src/debug_gui/DebugGui.h
        src/gui/Hotbar.cpp
        src/gui/Hotbar.h
        src/player/ItemStack.cpp
        src/player/ItemStack.h
        src/Config.cpp
        src/Config.h
)

include_directories("dependencies")
include_directories("shaders")
include_directories("resources")

add_subdirectory(dependencies/external/glfw)
add_subdirectory(dependencies/external/glm)

find_package(OpenGL REQUIRED)

add_executable(voxel_engine ${SOURCE_FILES})

target_link_libraries(voxel_engine ${OPENGL_LIBRARIES} glfw)