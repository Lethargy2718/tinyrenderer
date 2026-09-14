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

add_compile_options(-Wall -Wextra)

# tga
add_library(tgaimage STATIC lib/tgaimage.c)
target_include_directories(tgaimage PUBLIC lib include)

# obj
add_library(objloader STATIC lib/obj_loader.c)
target_include_directories(objloader PUBLIC lib include)

# main
add_executable(tinyrenderer src/main.c)
target_include_directories(tinyrenderer PRIVATE src)
target_link_libraries(tinyrenderer PRIVATE tgaimage objloader m)