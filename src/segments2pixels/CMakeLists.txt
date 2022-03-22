find_package(PNG REQUIRED)

# we need b/c we are dynamically listing the required libs for the Rust wrapper
# and without this we would be missing the system libs(eg boost, libpng, etc)
# ie all those obtained using find_package b/c IMPORTED are LOCAL by default
# cf https://gitlab.kitware.com/cmake/cmake/-/issues/17256
# MAYBE ALIAS_GLOBAL?
set_target_properties(PNG::PNG PROPERTIES IMPORTED_GLOBAL TRUE)

add_library(segments2pixels
    segments2pixels.cpp
)

target_link_libraries(segments2pixels
    PRIVATE
    PNG::PNG
    cimg
    absl::strings
    fmt::fmt
    glog::glog

    resources
    utils_files
    utils_rle
    verilog_defines
)

target_include_directories(segments2pixels
    INTERFACE
    # it is a bit more explicit to #include segments2pixels/segments2pixels.h
    # rather than segments2pixels.h directly
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
