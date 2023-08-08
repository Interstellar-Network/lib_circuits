################################################################################
find_package(PNG REQUIRED)

# we need b/c we are dynamically listing the required libs for the Rust wrapper
# and without this we would be missing the system libs(eg boost, libpng, etc)
# ie all those obtained using find_package b/c IMPORTED are LOCAL by default
# cf https://gitlab.kitware.com/cmake/cmake/-/issues/17256
# MAYBE ALIAS_GLOBAL?
set_target_properties(PNG::PNG PROPERTIES IMPORTED_GLOBAL TRUE)

################################################################################

add_library(drawable
    bbox.cpp
    drawable_digit_png.cpp
    drawable.cpp
    map_color_to_segid.cpp
    points.cpp
)

target_link_libraries(drawable
    PUBLIC
    absl::flat_hash_map

    PRIVATE
    glog::glog
    PNG::PNG
    Boost::filesystem
    cimg

    resources
    utils_files
)

target_include_directories(drawable
    INTERFACE
    # it is a bit more explicit to #include segments2pixels/segments2pixels.h
    # rather than segments2pixels.h directly
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
