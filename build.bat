

rmdir /S /Q build 2> nul
mkdir build
pushd build
cmake -A x64 -DCMAKE_BUILD_TYPE=relwithdebinfo ..
cmake --build . --config relwithdebinfo -- /m:%NUMBER_OF_PROCESSORS%
echo cd build && ctest -C relwithdebinfo --output-on-failure --output-junit report.xml
popd

rem pause