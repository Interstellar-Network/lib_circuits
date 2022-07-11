# we compile using a system-installed by default b/c easier for CI(and packaging)
# but compiling from source CAN be useful while working with ABC interfacing
if(NOT DEP_YOSYS_COMPILE_FROM_SOURCE)

################################################################################

# TODO make it optional based on option
find_library(YOSYS_LIBRARY yosys REQUIRED)
find_path(YOSYS_INCLUDE_DIR kernel/yosys.h REQUIRED
        # SHOULD be installed into "/usr/include/yosys/kernel/yosys.h"
        # so remove "yosys" part
        PATH_SUFFIXES yosys)

add_library(yosys::yosys SHARED IMPORTED GLOBAL)
set_property(TARGET yosys::yosys PROPERTY
             IMPORTED_LOCATION "${YOSYS_LIBRARY}")
target_include_directories(yosys::yosys INTERFACE "${YOSYS_INCLUDE_DIR}")
# "kernel/yosys.h:75:4: error: It looks like you are trying to build Yosys without the config defines set."
target_compile_definitions(yosys::yosys INTERFACE _YOSYS_)

return()

else(NOT DEP_YOSYS_COMPILE_FROM_SOURCE)

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

add_library(yosys::yosys ALIAS libyosys)

endif(NOT DEP_YOSYS_COMPILE_FROM_SOURCE)