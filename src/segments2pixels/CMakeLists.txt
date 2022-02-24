add_library(segments2pixels
    segments2pixels.cpp
)

target_link_libraries(segments2pixels
    PRIVATE
    lodepng
    cimg
    X11
    absl::strings

    resources
    utils_files
)

target_include_directories(segments2pixels
    INTERFACE
    # it is a bit more explicit to #include segments2pixels/segments2pixels.h
    # rather than segments2pixels.h directly
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
