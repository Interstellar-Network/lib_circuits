add_library(verilog_compiler
    verilog_compiler.cpp
)

target_link_libraries(verilog_compiler
    PRIVATE
    yosys
    absl::strings
    glog::glog

    resources
)

target_include_directories(verilog_compiler
    INTERFACE
    # it is a bit more explicit to #include verilog/verilog_compiler.h
    # rather than verilog_compiler.h directly
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
