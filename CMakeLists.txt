cmake_minimum_required(VERSION 3.16)
project(tinyrenderer_c C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()

add_library(tgaimage STATIC lib/tgaimage.c)
target_include_directories(tgaimage PUBLIC lib)

add_compile_options(-Wall -Wextra)

add_executable(tinyrenderer src/main.c)
target_include_directories(tinyrenderer PRIVATE src)
target_link_libraries(tinyrenderer PRIVATE tgaimage m)