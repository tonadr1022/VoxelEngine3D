## A 3D Voxel Engine Implementation

### Build and Run With CMake
MacOS:
```
sh build_run.sh
```



### Tentative Controls:
* WASD - Move
* R - Up
* F - Down
* hold B - unlock mouse
* scroll - shift hotbar item
* hold left click - break block
* hold right click - place block
* Left bracket - shift hotbar left 1 item
* Right bracket - shift hotbar right 1 item
* minus - shift hotbar by 10 items left (only 35ish items, inventory is 100)
* equals - shift hotbar by 10 items right

Block Ray Casting: http://www.cse.yorku.ca/~amana/research/grid.pdf


# Features
- Standard block placing and breaking
- Colored lights (not found in Minecraft!)


# Tech Features I've Learned
### C++
This is my first large project using the language. As of getting rather far into the project, 
I haven't formally learned a language that requires memory management (C, C++, etc), which made 
it difficult to debug at times.

### Multithreading
I had learned multithreading at a surface level in the past, but this was my first time using it extensively
to speed up data processing, particularly through terrain, structure, lighting, and mesh generation for chunks.

### Bit manipulation
Up to the point of working on this project, I had not formally learned bit manipulation. I utilized it
to pack data to send to the GPU for rendering and to reduce memory usage. For example, the lighting value
for a block contains 3 values between 0-15, so I packed them into 16-bits both to send to the GPU and for
chunk data storage. Prior to this, I was using glm::ivec3, which uses 6-fold the memory (12 bytes vs 2).
When I first implemented colored lights, my memory usage quintupled, but with this optimization, it brought the increase
down to two-fold. The other main bit manipulation used concerned packing all data necessary to render a face.
My ChunkVertex struct contains two 32-bit unsigned integers rather than floats for every value, since the position values
in a chunk range from 0-32 (5 bits each), UV-coordinates are 2 bits total, ambient occlusion is 4 bits, and the texture index
is 8 bits.

### Memory Management/Optimization
Prior to this C++ project, I had never worked with pointers. This led to many points of confusion as to how and when memory is allocated.
One particular struggle occurred when my compile time slowed to a crawl after I initialized a massive array at compile time without 
realizing. Dealing with large amounts of data to maximize performance forced me to learn to reduce memory usage whenever possible.
Aside from reducing data size through bit manipulation, I switched from always allocating space for light data on chunk creation to only
doing so when a light level is set for the chunk. I realized the trade-off for an extra if statement or two compared to hundreds of megabytes
of data in memory was worth it, a concept I had never considered before this project.

# Inspiration
- Idea for packing colored lights into a 16-bit integer came from this video: https://www.youtube.com/watch?v=edaaFUflusk.
- Ambient Occlusion for voxels concepts: https://0fps.net/2013/07/03/ambient-occlusion-for-minecraft-like-worlds/


# TODO
- Separate water mesh for water shader, may require chunk mesh batching to reduce draw calls
- Greedy meshing
- Sunlight
- Skybox/background
- Day and night cycle
- Shadow mapping
- Physics for movement/AABB collision detection
- More block types
- Biomes system
- Fix breaking/placing multicolored lights and increase speed to eliminate slight frame drops