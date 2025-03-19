#!/bin/bash

rm -rf build
mkdir -p build

# pushd build
cd build

# -DCMAKE_EXPORT_COMPILE_COMMANDS=ON option is implemented only by Makefile Generators and Ninja Generators.
cmake -DCMAKE_BUILD_TYPE=relwithdebinfo -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cmake --build . --config relwithdebinfo -- -j$(nproc)
echo "cd build && ctest -C relwithdebinfo --output-on-failure --output-junit report.xml"
# popd
cd ..

# echo continue && read -n 1
