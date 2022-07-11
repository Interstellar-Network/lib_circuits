include(${PROJECT_SOURCE_DIR}/3rd_party/_conan.cmake)

################################################################################

include(${CMAKE_BINARY_DIR}/conan.cmake)


conan_cmake_configure(REQUIRES fmt/8.1.1
                      GENERATORS cmake_find_package)

# NO!
# FAIL:
# ERROR: Missing prebuilt package for...
# - We DO NOT care if the package was built with gcc even if locally we are using clang
# - We WANT to always use Release libs even when building Debug locally(SHOULD be configurable)
# conan_cmake_autodetect(settings)

message(WARNING "settings : ${settings}")

conan_cmake_install(PATH_OR_REFERENCE .
                    # NO! we WANT the prebuilt binary
                    # BUILD missing
                    REMOTE conancenter
                    # SETTINGS ${settings}
)

# cf build/Findfmt.cmake for the vars
find_package(fmt REQUIRED)

return()

################################################################################

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY  https://github.com/fmtlib/fmt.git
    GIT_TAG      8.1.1
)

FetchContent_MakeAvailable(fmt)
