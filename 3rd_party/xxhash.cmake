include(FetchContent)

FetchContent_Declare(
    xxhash
    GIT_REPOSITORY  https://github.com/Cyan4973/xxHash.git
    GIT_TAG      v0.8.1
    SOURCE_SUBDIR cmake_unofficial
)

FetchContent_MakeAvailable(xxhash)
