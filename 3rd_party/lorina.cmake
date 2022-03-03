# TODO?
# and PATCH mockturtle CMake to use a up-to-date lorina?
# OR just add it temporarily and if hello_cli can parse the verilog, act?
if(NOT TARGET lorina)

include(FetchContent)

FetchContent_Declare(
    lorina
    # TODO(deps) replace by internal mirror
    GIT_REPOSITORY https://github.com/hriener/lorina.git
    # master 2022-02-15
    # NOTE: the last release/tag is v0.2 released this Oct 18, 2018
    GIT_TAG        79bde7375008b587b5c2f99ae063e2daf3a3154d
)

# other options default to OFF already; but repeat just in case for consistency
option(LORINA_EXAMPLES "Build examples" OFF) # defaults to ON
option(LORINA_TEST "Build tests" OFF)
option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" OFF)

FetchContent_MakeAvailable(lorina)

endif()  # if(NOT TARGET lorina)