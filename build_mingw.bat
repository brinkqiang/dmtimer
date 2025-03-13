

rmdir /S /Q build 2> nul
mkdir build
pushd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=relwithdebinfo ..
cmake --build . --config relwithdebinfo
popd

rem pause