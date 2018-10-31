rmdir /S /Q build
mkdir build
pushd build
cmake -DCMAKE_BUILD_TYPE=relwithdebinfo ..
cmake --build .
popd