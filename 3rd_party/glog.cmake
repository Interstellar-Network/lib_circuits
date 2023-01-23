# ##############################################################################
include(FetchContent)

# only way to disable glog tests...
# https://github.com/google/glog/pull/200
# "clean workaround" apparently
set(BUILD_TESTING_SAVED "${BUILD_TESTING}")
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glog

    # v0.5.0 released this May 08, 2021
    # but it does NOT compile with clang 13 and all the warnings
    GIT_REPOSITORY https://github.com/google/glog.git

    GIT_TAG v0.6.0
)

FetchContent_MakeAvailable(glog)

set(BUILD_TESTING "${BUILD_TESTING_SAVED}" CACHE BOOL "" FORCE)