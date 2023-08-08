# ###############################################################################

include(${PROJECT_SOURCE_DIR}/3rd_party/tests/googletest.cmake)

# NOTE: enable_testing() SHOULD be called from the root according to the docs

# ###############################################################################
add_subdirectory(data)

# ###############################################################################

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
        GTest::gtest
        GTest::gtest_main
        glog::glog
        ${DEPENDENCY_LIBS}
    )

    target_compile_definitions(${test_cpp_name} PRIVATE
    )

    # TODO DRY, and call from here instead of duplicating
    # gtest_discover_tests(${test_cpp_name})
endfunction()

# NOTE: resources is linked PRIVATE by circuit_lib(in src/);
# but relinking avoids:
# - making it public which would not be right
# - copy-pasting resources.h.in in tests/
# NOTE: gtest_discover_tests requires cmake 3.10
add_google_test(abc_test "abc-wrapper test_resources absl::strings utils_files")
gtest_discover_tests(abc_test)
add_google_test(full_pipeline_test "circuit_lib test_resources absl::strings utils_files")
gtest_discover_tests(full_pipeline_test)
add_google_test(segments2pixels_test "segments2pixels test_resources absl::strings utils_files GTest::gmock")
gtest_discover_tests(segments2pixels_test)
add_google_test(verilog_compiler_test "verilog_compiler resources absl::strings utils_files")
gtest_discover_tests(verilog_compiler_test)

# ###############################################################################
if(interstellar_lib_circuits_HAS_CLIS)
    # basic cli(not googletest); but that still works fine
    # add_google_test(hello_cli "circuit_lib lorina absl::strings resources")
    add_google_test(cli_display_skcd "circuit_lib absl::flags absl::flags_parse")
    add_google_test(cli_generic_skcd "circuit_lib absl::flags absl::flags_parse")
endif(interstellar_lib_circuits_HAS_CLIS)

# ###############################################################################
