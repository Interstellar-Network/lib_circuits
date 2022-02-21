add_library(segments2pixels
    segments2pixels.cpp
)

target_link_libraries(segments2pixels
    PRIVATE
    resources
)

target_include_directories(segments2pixels
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
