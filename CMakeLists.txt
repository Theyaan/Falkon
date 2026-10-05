set(OpenGL_GL_PREFERENCE GLVND)
cmake_minimum_required(VERSION 3.10)
project(r36s-falkon-controller VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

# Virtual controller daemon
add_executable(r36s-falkon-controller src/virtual_input.cpp)
target_include_directories(r36s-falkon-controller PRIVATE src ${SDL2_INCLUDE_DIRS})
target_link_libraries(r36s-falkon-controller PRIVATE ${SDL2_LIBRARIES})
