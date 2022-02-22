################################################################################
# prerequisite : Boost: filesystem
find_package(Boost COMPONENTS filesystem REQUIRED)

################################################################################

add_library(utils_files
    utils_files.cpp
)

target_link_libraries(utils_files
    PRIVATE
    glog::glog
    ${Boost_FILESYSTEM_LIBRARY}
    absl::strings
    absl::random_random
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