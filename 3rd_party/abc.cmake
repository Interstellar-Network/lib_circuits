# abc
# NOTE: yosys has a "frontend" for abc, but it's basically calling the yosys-abc executable
# so instead we compile libabc, and use the c(++) api

################################################################################

# abc: prereq: make
# use that to avoid a cryptic error around "extract_var" in abc's CMake
# when building on a server without make(eg CI)
find_program(MAKE_EXE make)
if(NOT MAKE_EXE)
message(FATAL_ERROR "make not found(abc prereq)")
endif()

################################################################################

include(FetchContent)

FetchContent_Declare(
    abc
    GIT_REPOSITORY https://github.com/berkeley-abc/abc.git
    # NOTE: there is no release/tag...
    # latest + yosys[0.8-0.14] FAIL garbling
    # Commits on Feb 18, 2022
    # GIT_TAG        31519bd6d6c9cff4691019f72e0faf72e37bde88
    # Commits on Mar 11, 2018
    GIT_TAG         c339c6f7f1d740558e1cfb4d6ba6eed818edb654
)

FetchContent_MakeAvailable(abc)

target_compile_options(libabc-pic PRIVATE
    -Wno-unused-parameter
    -Wno-format
    -Wno-empty-body
    # [build] /usr/include/features.h:187:3: error: "_BSD_SOURCE and _SVID_SOURCE are deprecated, use _DEFAULT_SOURCE" [-Werror,-W#warnings]
    # [build] # warning "_BSD_SOURCE and _SVID_SOURCE are deprecated, use _DEFAULT_SOURCE"
    -Wno-cpp
    -Wno-misleading-indentation
    -Wno-self-assign
    -Wno-implicit-const-int-float-conversion
)