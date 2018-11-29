cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

option(USE_PKGCONFIG "USE_PKGCONFIG" OFF)
option(USE_Q32 "USE Q32" OFF)

if(MINGW OR APPLE)
    find_package(OpenMP)
endif()
if(MINGW)
    find_package(Threads)
endif()

if(USE_PKGCONFIG)
    find_package(PkgConfig)
    pkg_search_module(MAGICK REQUIRED Magick++)
    pkg_search_module(LCMS2 REQUIRED lcms2)
    add_compile_options(${MAGICK_CFLAGS})
    #add_compile_options(${MAGICK_STATIC_CFLAGS})
    include_directories(${MAGICK_INCLUDE_DIRS} ${LCMS2_INCLUDE_DIRS})
else()
    find_package(LCMS2 REQUIRED)
    include_directories(${LCMS2_INCLUDE_DIR})
    find_package(ImageMagick COMPONENTS MagickCore REQUIRED)
    find_package(ImageMagick COMPONENTS MagickWand REQUIRED)
    find_package(ImageMagick COMPONENTS Magick++ REQUIRED)
    include_directories(${ImageMagick_INCLUDE_DIRS})
    add_compile_options(${MAGICK_CFLAGS})
    if(USE_Q32)
        add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=32)
    else()
        add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=16)
    endif()
        add_definitions(-DMAGICKCORE_HDRI_ENABLE=1)
endif()
