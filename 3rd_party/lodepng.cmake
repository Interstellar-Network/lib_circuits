# lodepng
# why not libpng? b/c there is no official c++ support and the code needed
# to juste open a png if awful see https://github.com/glennrp/libpng/blob/master/pngtest.c
# Yes libpng is faster; but that does not matter b/c we reuse circuits and master files.

include(FetchContent)

FetchContent_Declare(
    lodepng_fetch
    # 5601b82 on Jan 10
    GIT_REPOSITORY  https://github.com/lvandeve/lodepng.git
    GIT_TAG     5601b82
)

# NOT a CMake project so no option etc

FetchContent_MakeAvailable(lodepng_fetch)

# NOT a CMake project, add the minimal lib manually
add_library(lodepng
    ${lodepng_fetch_SOURCE_DIR}/lodepng.cpp
)

target_include_directories(lodepng
    INTERFACE
    ${lodepng_fetch_SOURCE_DIR}
)