add_library(utils_rle
    encode_rle.cpp
)

target_link_libraries(utils_rle
    PRIVATE

    PUBLIC
)

target_include_directories(utils_rle
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)
