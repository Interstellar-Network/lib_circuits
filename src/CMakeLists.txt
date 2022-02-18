add_library(circuit_lib
    lib.cpp
)

target_link_libraries(circuit_lib
    PRIVATE
    # TODO(mockturtle)
    # mockturtle
    resources
)