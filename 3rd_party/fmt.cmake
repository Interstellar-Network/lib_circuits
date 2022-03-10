include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY  https://github.com/fmtlib/fmt.git
    GIT_TAG      8.1.1
)

FetchContent_MakeAvailable(fmt)

# cmake --build . --target help | grep fmt
set(TARGETS_LIST
fmt
)

foreach(TARGET IN LISTS TARGETS_LIST)
    set_target_properties(${TARGET} PROPERTIES
      # disable auto-running clang-tidy
      CXX_CLANG_TIDY ""
  )
endforeach()