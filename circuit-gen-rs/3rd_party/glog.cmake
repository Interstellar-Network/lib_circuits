# ##############################################################################
include(FetchContent)

# only way to disable glog tests...
# https://github.com/google/glog/pull/200
# "clean workaround" apparently
set(BUILD_TESTING_SAVED "${BUILD_TESTING}")
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glog

    # latest release is v0.6 but it is from 2022 and does not compile with Clang 15
    GIT_REPOSITORY https://github.com/google/glog.git
    GIT_TAG 3a0d4d22c5ae0b9a2216988411cfa6bf860cc372
)

FetchContent_MakeAvailable(glog)

set(BUILD_TESTING "${BUILD_TESTING_SAVED}" CACHE BOOL "" FORCE)

target_compile_options(glog_internal
    PRIVATE

    # glog-src/src/symbolize.cc:538:12: error: variable 'num_maps' set but not used [-Werror,-Wunused-but-set-variable]
    # unsigned num_maps = 0;
    $<$<CXX_COMPILER_ID:Clang>:
    -Wno-unused-but-set-variable
    >
)