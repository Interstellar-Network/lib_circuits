add_library(verilog_defines
    verilog_defines.cpp
)

target_link_libraries(verilog_defines
    PRIVATE
    absl::strings
    absl::flat_hash_map
)

target_include_directories(verilog_defines
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
