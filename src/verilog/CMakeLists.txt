add_library(verilog_compiler
    verilog_compiler.cpp
)

target_link_libraries(verilog_compiler
    PRIVATE
    yosys
    absl::strings
)

target_include_directories(verilog_compiler
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
