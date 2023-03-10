cmake_minimum_required(VERSION 3.10)
include(FindPkgConfig)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(WideBot VERSION 1.0)

add_executable( WideBot
  src/main.cpp
  src/widebot.cpp
)

target_include_directories(WideBot PRIVATE include)

add_subdirectory(lib/sleepy-discord)
target_link_libraries(WideBot sleepy-discord)

add_subdirectory(lib/widepeepolib)
target_link_libraries(WideBot widepeepolib)

pkg_check_modules(CURLPP REQUIRED curlpp)
target_link_libraries(WideBot ${CURLPP_LDFLAGS})

find_package(ImageMagick REQUIRED COMPONENTS Magick++ MagickCore MagickWand)
if (ImageMagick_FOUND)
  message(STATUS "ImageMagick Libraries: " ${ImageMagick_LIBRARIES})
  message(STATUS "ImageMagick Include Directories: " ${ImageMagick_INCLUDE_DIRS})
endif (ImageMagick_FOUND)
add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
add_definitions( -DMAGICKCORE_HDRI_ENABLE=1 )
target_link_libraries(WideBot ${ImageMagick_LIBRARIES})
target_include_directories(WideBot PRIVATE ${ImageMagick_INCLUDE_DIRS})

configure_file(token.txt token.txt COPYONLY)
