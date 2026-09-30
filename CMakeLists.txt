cmake_minimum_required(VERSION 3.16)
project(tinyrenderer_c C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(OpenMP REQUIRED)

add_compile_options(-Wall -Wextra)
set(CMAKE_C_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")

# sanitizers only in Debug
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()

add_executable(tinyrenderer
    src/main.c
    src/tgaimage.c
    src/model.c
    src/render.c
    src/line.c
    src/triangle.c
    src/color.c
)

target_include_directories(tinyrenderer PRIVATE include)
target_link_libraries(tinyrenderer PRIVATE m OpenMP::OpenMP_C)