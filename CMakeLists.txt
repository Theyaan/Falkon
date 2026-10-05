set(OpenGL_GL_PREFERENCE GLVND)
cmake_minimum_required(VERSION 3.10)
project(falkon-for-arkos VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

# Virtual controller daemon
add_executable(falkon-for-arkos src/virtual_input.cpp)
target_include_directories(falkon-for-arkos PRIVATE src ${SDL2_INCLUDE_DIRS})
target_link_libraries(falkon-for-arkos PRIVATE ${SDL2_LIBRARIES})
