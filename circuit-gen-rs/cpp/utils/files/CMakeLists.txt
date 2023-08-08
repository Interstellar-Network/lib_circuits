################################################################################
# prerequisite : Boost: filesystem
# STATIC b/c that avoids dealing with mismatch SOVERSION local(eg 1.71) and in
# Docker rust(eg 1.74) which causes "Libraries not found"
# Yes it is NOT the proper way to deal with it.
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS filesystem REQUIRED)

# we need b/c we are dynamically listing the required libs for the Rust wrapper
# and without this we would be missing the system libs(eg boost, libpng, etc)
# ie all those obtained using find_package b/c IMPORTED are LOCAL by default
# cf https://gitlab.kitware.com/cmake/cmake/-/issues/17256
# MAYBE ALIAS_GLOBAL?
set_target_properties(Boost::filesystem PROPERTIES IMPORTED_GLOBAL TRUE)

################################################################################

add_library(utils_files
    utils_files.cpp
)

target_link_libraries(utils_files
    PRIVATE
    glog::glog
    absl::strings
    absl::random_random

    PUBLIC
    # public that way linking with utils_file and #include "utils_file.h" will "just work"
    Boost::filesystem
)

target_include_directories(utils_files
    INTERFACE
    # it is a bit more explicit to #include utils/files/utils_files.h
    # rather than utils_files.h directly
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_compile_definitions(utils_files
    # TODO should this be public? probabably yes b/c filesystem.hpp is included by the internal .h
    PUBLIC BOOST_FILESYSTEM_NO_DEPRECATED
)