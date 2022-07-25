## Dev

If you want the "quick and easy" setup like in CI:

~~- install Conan(C++ package manager) **and its tools** eg `pip3 install conan_package_tools`
    - `conan profile new default --detect`
    - IMPORTANT `conan profile update settings.compiler.libcxx=libstdc++11 default`
    This is really important, without this you will get "undefined symbols" and other linker errors with dependencies(eg Protobuf).~~
- install Yosys and ABC, cf .github/workflows/cpp.yml for details

Else: you CAN set CMake options to compile ABC and Yosys from source.

## TODO

- when generating the circuits(.v and/or .skcd) we SHOULD also pacakge(serialize) what is needed for the display shader
eg is it a 2*1 render(message), a 1*10(pinpad), other?