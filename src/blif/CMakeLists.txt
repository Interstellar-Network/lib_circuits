add_library(blif_parser
    blif_parser.cpp
)

target_include_directories(blif_parser
    INTERFACE

    # it is a bit more explicit to #include blif/blif_parser.h
    # rather than blif_parser.h directly
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(blif_parser
    PUBLIC
    absl::flat_hash_map

    PRIVATE
    fmt::fmt
    absl::strings
)