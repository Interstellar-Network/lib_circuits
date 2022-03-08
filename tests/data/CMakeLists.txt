# TODO handle installation/packaging (eg .deb and system install)
set(RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
configure_file(test_resources.h.in "${PROJECT_BINARY_DIR}/generated/test_resources.h" @ONLY)

add_library(test_resources INTERFACE)
target_include_directories(test_resources
    INTERFACE "${PROJECT_BINARY_DIR}/generated"
)