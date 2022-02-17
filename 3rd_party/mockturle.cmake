if(NOT TARGET mockturtle)

include(FetchContent)

FetchContent_Declare(
    mockturtle
    # TODO(deps) replace by internal mirror
    GIT_REPOSITORY https://github.com/lsils/mockturtle.git
    # master 2022-02-17
    # NOTE: the last release/tag is 0.2 from Feb 16, 2021
    GIT_TAG        e147c525ce5924fbe0a751
)

option(MOCKTURTLE_EXAMPLES "Build examples" OFF) # defaults to ON
# other options default to OFF already; but repeat just in case for consistency
option(MOCKTURTLE_TEST "Build tests" OFF)
option(MOCKTURTLE_EXPERIMENTS "Build experiments" OFF)
option(BILL_Z3 "Enable Z3 interface for bill library" OFF)
option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" OFF)
option(ENABLE_MATPLOTLIB "Enable matplotlib library in experiments" OFF)

FetchContent_MakeAvailable(mockturtle)

endif()  # if(NOT TARGET mockturtle )