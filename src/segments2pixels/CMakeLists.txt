find_package(PNG REQUIRED)

add_library(segments2pixels
    segments2pixels.cpp
)

target_link_libraries(segments2pixels
    PRIVATE
    PNG::PNG
    cimg
    X11 # TODO remove
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
