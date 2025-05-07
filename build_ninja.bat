

rmdir /S /Q build 2> nul
mkdir build
pushd build
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=relwithdebinfo ..
cmake --build . --config relwithdebinfo
popd

rem pause