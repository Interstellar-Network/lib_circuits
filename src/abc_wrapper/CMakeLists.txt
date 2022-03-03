
# wrapper for abc, to be able to disable the warning as needed without affecting the rest of the code
add_library(abc-wrapper
    abc_wrapper.cpp
)

target_link_libraries(abc-wrapper
    PRIVATE
    libabc-pic
    absl::strings
    glog::glog

    resources
)

target_include_directories(abc-wrapper
    INTERFACE
    # it is a bit more explicit to #include abc_wrapper/abc_wrapper.h
    # rather than abc_wrapper.h directly
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_compile_options(abc-wrapper
    PRIVATE
    # b/c of abc_global.h and others
    -Wno-unused-parameter

    $<$<CXX_COMPILER_ID:GNU>:
        -Wno-cast-function-type
        -Wno-shift-negative-value
        >
)
