include(FetchContent)
FetchContent_Declare(
  googletest
  # Commits on Feb 14, 2022
  URL https://github.com/google/googletest/archive/ea55f1f52c489535f0d3b583c81529762c9cb5ea.zip
)

# "For Windows: Prevent overriding the parent project's compiler/linker settings"
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

# cmake --build . --target help | grep gtest
set(TARGETS_LIST
gtest
gtest_main
)

foreach(TARGET IN LISTS TARGETS_LIST)
    set_target_properties(${TARGET} PROPERTIES
      # disable auto-running clang-tidy
      CXX_CLANG_TIDY ""
  )
endforeach()