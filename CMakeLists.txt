
cmake_minimum_required(VERSION 3.16)

project(triangles LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use pkg-config to find SDL3
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)

add_executable(triangles triangles.cpp Matrix4f.cpp)

# Include SDL3 headers
target_include_directories(triangles PRIVATE ${SDL3_INCLUDE_DIRS})

# Link SDL3 libraries
target_link_libraries(triangles PRIVATE ${SDL3_LIBRARIES})

# Pass any extra compiler flags from pkg-config
target_compile_options(triangles PRIVATE ${SDL3_CFLAGS_OTHER})
