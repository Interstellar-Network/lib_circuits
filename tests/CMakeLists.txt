################################################################################

include(${PROJECT_SOURCE_DIR}/3rd_party/googletest.cmake)

# NOTE: enable_testing() SHOULD be called from the root according to the docs

################################################################################

# TODO passed list of cpp test files(eg the .cpp in tests/)
function(add_google_test test_cpp_name DEPENDENCY_LIBS)

    # space separated string -> list
    # without this target_link_libraries does not work correctly
    separate_arguments(DEPENDENCY_LIBS)

    add_executable(${test_cpp_name} ${test_cpp_name}.cpp)

    target_compile_options(${test_cpp_name} PRIVATE

    )

    target_include_directories(${test_cpp_name} PRIVATE

    )

    target_link_libraries(${test_cpp_name}
        gtest_main
        ${DEPENDENCY_LIBS}
    )

    target_compile_definitions(${test_cpp_name} PRIVATE
    )

endfunction()

# NOTE: resources is linked PRIVATE by circuit_lib(in src/);
# but relinking avoids:
# - making it public which would not be right
# - copy-pasting resources.h.in in tests/
add_google_test(verilog_compiler_test "verilog_compiler resources absl::strings utils_files")
 # NOTE: requires cmake 3.10
gtest_discover_tests(verilog_compiler_test)

# basic cli(not googletest); but that still works fine
# add_google_test(hello_cli "circuit_lib lorina absl::strings resources")
add_google_test(cli_display_skcd "circuit_lib absl::flags absl::flags_parse")
add_google_test(cli_generic_skcd "circuit_lib absl::flags absl::flags_parse")

################################################################################
