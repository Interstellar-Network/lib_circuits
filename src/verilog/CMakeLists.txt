add_library(verilog_compiler
    verilog_compiler.cpp
)

target_link_libraries(verilog_compiler
    PRIVATE
    # IMPORTANT: Yosys passes are "auto registered" which means that they are NOT
    # directly used. Which causes them to be optmized out when static linking
    # eg "ERROR: No such command: techmap"
    # We MUST use "whole-archive" when using STATIC to make it work.
    # -Wl,--whole-archive
    libyosys
    # -Wl,--no-whole-archive

    absl::strings
    absl::synchronization
    glog::glog

    resources
)

target_include_directories(verilog_compiler
    INTERFACE
    # it is a bit more explicit to #include verilog/verilog_compiler.h
    # rather than verilog_compiler.h directly
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
