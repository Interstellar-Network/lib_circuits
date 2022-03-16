################################################################################
# yosys

# Usuful only when dev/debug yosys-related code
# Allows to rebuild Yosys when its Makefile is modified
option(YOSYS_ENABLE_DEBUG "ENABLE_DEBUG for Yosys Makefile" OFF)

option(YOSYS_ENABLE_TESTING "ENABLE_DEBUG for Yosys/LIBYOSYS" OFF)
option(YOSYS_BUILD_SHARED_LIBS "Yosys BUILD_SHARED_LIBS" ON)

################################################################################

include(FetchContent)

# NOTE: using our own fork until (and if) the CMake support is merged upstream
FetchContent_Declare(
    yosys_fetch
    # TODO use master after the PR is merged
    GIT_REPOSITORY  https://github.com/Interstellar-Network/yosys.git
    GIT_TAG     5cd567d
)

FetchContent_MakeAvailable(yosys_fetch)