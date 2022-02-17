################################################################################
# yosys
# Does NOT support CMake. It used to be compiled with a custom DIY CMakeLists.txt
# but that is not really maintainable so replaced by a ExternalProject.
# NOTE: FetchContent requires support for CMake, so we must use ExternalProject
# instead.
# ExternalProject alone does not allow to do eg add_library(yosys ...): "Imported target "yosys" includes non-existent path"
# https://stackoverflow.com/questions/45516209/cmake-how-to-use-interface-include-directories-with-externalproject

# Usuful only when dev/debug yosys-related code
# Allows to rebuild Yosys when its Makefile is modified
option(yosys_BUILD_ALWAYS "BUILD_ALWAYS for yosys step(useful to dev/debug yosys related code)" OFF)

find_program(MAKE_EXE make)
if(NOT MAKE_EXE)
message(FATAL_ERROR "make not found(abc prereq)")
endif()

find_program(BISON bison)
if(NOT BISON)
    message(FATAL_ERROR "bison not found!")
endif()

find_program(FLEX flex)
if(NOT FLEX)
    message(FATAL_ERROR "flex not found!")
endif()

find_program(PYTHON3_EXE python3)
if(NOT PYTHON3_EXE)
    message(FATAL_ERROR "python3 not found!")
endif()

find_package(TCL REQUIRED)

find_path(READLINE_INCLUDE_PATH NAMES readline/readline.h)
find_library(READLINE_LIBRARY NAMES readline)

if(NOT READLINE_INCLUDE_PATH)
    message(FATAL_ERROR "readline not found!(apt install libreadline-dev)")
endif()

# TODO
# include(FindFFI.cmake)
# if(NOT FFI_FOUND)
#     message(FATAL_ERROR "libffi!(apt install libffi-dev)")
# endif()

include(FetchContent)
include(ExternalProject)

# https://stackoverflow.com/questions/55174921/cmake-how-to-build-an-external-project-using-all-cores-on-nix-systems
include(ProcessorCount)
ProcessorCount(N)

# Using FetchContent means we will download at configure-time
FetchContent_Declare(yosys_dl
    # TODO use either local source, or a private mirror (ie in case official repo is taken down)
    GIT_REPOSITORY  https://github.com/YosysHQ/yosys.git
    GIT_TAG yosys-0.14
)

FetchContent_MakeAvailable(yosys_dl)

# Build
ExternalProject_Add(yosys_ext
    SOURCE_DIR ${yosys_dl_SOURCE_DIR}
    UPDATE_DISCONNECTED ON
    # not needed; and causes eg "cp: cannot create regular file '/usr/local/bin/yosys': Permission denied"
    # TODO "make install" should work with "PREFIX" in makefile.conf; in that case should probably remove LIBDIR and adjust IMPORTED_LOCATION
    INSTALL_COMMAND ""
    # https://github.com/YosysHQ/yosys#building-from-source
    CONFIGURE_COMMAND ""
    # NO QUOTES else "bash: /usr/bin/make -j14: No such file or directory"
    # passing debug flags via CXXFLAGS does NOT work in Makefile.conf below...
    # $<$<CONFIG:Debug>:CXXFLAGS=-fno-limit-debug-info>  # NO; it breaks compilation for some reason...
    BUILD_COMMAND ${MAKE_EXE} -j${N}
    # TODO build out of source + make install?
    BUILD_IN_SOURCE ON
    # cleaner than using add_dependencies?
    BUILD_BYPRODUCTS ${yosys_dl_SOURCE_DIR}/libyosys.so
    BUILD_ALWAYS ${yosys_BUILD_ALWAYS}
)

# Yosys maintainer refuses to support a proper build system so we must modify the Makefile
# itself to pass config options...
# LIBDIR:
# By default libyosys.so's sonanme is set to "LIBDIR := $(PREFIX)/lib/$(PROGRAM_PREFIX)yosys"
# which causes the lib/exe to be linked with "/usr/local/lib/yosys/libyosys.so => not found"
# CHECK with eg "ldd ./circuit_display_gen_cli | grep yosys"
file(GENERATE OUTPUT ${yosys_dl_SOURCE_DIR}/Makefile.conf CONTENT
    # TODO pass correct CXX flags(at least use the same warnings than CMake)
    # ENABLE_DEBUG: NOT ENOUGH, also needs "fno-limit-debug-info"
    # TODO install; handle DATDIR ?
    "ENABLE_LIBYOSYS := 1\nENABLE_ABC := 1\nLIBDIR := ${yosys_dl_SOURCE_DIR}\nENABLE_DEBUG := $<IF:$<CONFIG:Debug>,1,0>\nPREFIX := ${yosys_dl_BINARY_DIR}\n"
)
ExternalProject_Add_StepDependencies(yosys_ext build ${yosys_dl_SOURCE_DIR}/Makefile.conf)

# Make the artifacts available to our project
add_library(yosys SHARED IMPORTED GLOBAL)

set_target_properties(yosys
    PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${yosys_dl_SOURCE_DIR}
    IMPORTED_LOCATION ${yosys_dl_SOURCE_DIR}/libyosys.so
)

target_compile_definitions(yosys
    INTERFACE
    # kernel/yosys.h:75:4: error: It looks like you are trying to build Yosys without the config defines set.          When building Yosys with a custom make system, make sure you set all the          defines the Yosys Makefile would set for your build configuration.
    # [build] #  error It looks like you are trying to build Yosys without the config defines set. \
    _YOSYS_
)