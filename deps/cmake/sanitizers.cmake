# dev option: sanitizer
# NOTE/ If ON, you should build with at least O1 else it is SLOW!
#
# "clang: error: invalid argument '-fsanitize=address' not allowed with '-fsanitize=memory'"
# "clang: error: invalid argument '-fsanitize=address' not allowed with '-fsanitize=thread'"
# "clang: error: invalid argument '-fsanitize=safe-stack' not allowed with '-fsanitize=leak'"
option(USE_ADDRESS_SANITIZER "Build with fsanitize: address, leak" OFF)
option(USE_THREAD_SANITIZER "Build with fsanitize: thread" OFF)
option(USE_MEMORY_SANITIZER "Build with fsanitize: memory" OFF)

if(USE_ADDRESS_SANITIZER)

# TODO is this still up-to-date?
# NOTE: this will build EVERYTHING with the sanitizer!
# no-sanitize=vptr: else lots of false positive regarding overriden virtual methods,
# and inheritance("which does not point to an object of type 'grpc::AuthContext' [...] object is of type 'grpc::SecureAuthContext'")
# PREREQUISITES: apt install libc++-dev libc++abi-dev
# We need to use clang's libc++ b/c of error with fsanitize=undefined
# in std::function, and possibly elsewhere
add_compile_options(
    -fsanitize=address
    -fsanitize=leak
    -fsanitize=undefined

    # "To get nicer stack traces in error messages add -fno-omit-frame-pointer"
    -fno-omit-frame-pointer
)

link_libraries(
    -fsanitize=address
    -fsanitize=leak
    -fsanitize=undefined
)

endif(USE_ADDRESS_SANITIZER)