@echo off

rem Create build directory
mkdir build
cd build

rem Run CMake
cmake ..

rem Build the project (assuming you are using Visual Studio)
msbuild my_project.sln /p:Configuration=Release

rem Run the executable
my_executable.exe
