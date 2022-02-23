add_library(skcd
    skcd.cpp
)

target_include_directories(skcd
    INTERFACE
    # it is a bit more explicit to #include skcd/skcd.h
    # rather than skcd.h directly
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(skcd
    PRIVATE
    interstellar_protos
    utils_files

    PUBLIC
    blif_parser
)