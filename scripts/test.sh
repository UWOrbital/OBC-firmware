mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Test
cmake --build .
ctest --verbose
