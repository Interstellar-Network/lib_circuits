################################################################################

add_subdirectory(abc_wrapper)
add_subdirectory(segments2pixels)
add_subdirectory(verilog)
add_subdirectory(verilog_defines)
add_subdirectory(utils/encode_rle/)
add_subdirectory(utils/files/)
add_subdirectory(blif)
add_subdirectory(skcd)
add_subdirectory(drawable)

################################################################################

add_library(circuit_lib
    circuit_lib.cpp
    # TODO move to separate libs, and under utils/
    utils_noncopyable.cpp
)

target_link_libraries(circuit_lib
    PUBLIC

    PRIVATE
    absl::strings

    abc-wrapper
    drawable
    segments2pixels
    verilog_compiler
    resources
    skcd
    utils_files
)

target_include_directories(circuit_lib
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)