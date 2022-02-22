################################################################################

add_subdirectory(abc_wrapper)
add_subdirectory(segments2pixels)
add_subdirectory(verilog)
add_subdirectory(utils/files/)

################################################################################

add_library(circuit_lib
    blif_parser.cpp
    circuit_lib.cpp
    skcd.cpp
    # TODO move to separate libs, and under utils/
    utils_noncopyable.cpp
)

target_link_libraries(circuit_lib
    PRIVATE
    # mockturtle # TODO?
    fmt
    xxhash
    absl::strings

    abc-wrapper
    segments2pixels
    verilog_compiler
    resources
    utils_files
)

target_include_directories(circuit_lib
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)