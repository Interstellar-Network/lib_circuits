# https://clang.llvm.org/docs/SourceBasedCodeCoverage.html

option(ENABLE_COVERAGE "Build for coverage support" OFF)

if(ENABLE_COVERAGE)

add_compile_options(
    -fprofile-instr-generate
    -fcoverage-mapping
)

link_libraries(
    -fprofile-instr-generate
)

endif(ENABLE_COVERAGE)