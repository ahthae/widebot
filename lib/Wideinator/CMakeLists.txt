cmake_minimum_required(VERSION 3.10)
include(FindPkgConfig)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(wideinator VERSION 1.0)

add_library(wideinator
  src/cmdline.c
  src/wideinator.cpp
)

target_include_directories(wideinator PUBLIC include)

find_package(ImageMagick REQUIRED COMPONENTS Magick++ MagickCore MagickWand)
if (ImageMagick_FOUND)
  message(STATUS "ImageMagick Libraries: " ${ImageMagick_LIBRARIES})
  message(STATUS "ImageMagick Include Directories: " ${ImageMagick_INCLUDE_DIRS})
endif (ImageMagick_FOUND)
add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
add_definitions( -DMAGICKCORE_HDRI_ENABLE=1 )
target_link_libraries(wideinator ${ImageMagick_LIBRARIES})
target_include_directories(wideinator PRIVATE ${ImageMagick_INCLUDE_DIRS})
