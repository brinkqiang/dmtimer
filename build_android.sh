#!/bin/bash

rm -rf build
mkdir -p build

# pushd build
cd build

# -DCMAKE_EXPORT_COMPILE_COMMANDS=ON option is implemented only by Makefile Generators and Ninja Generators.
echo download https://googledownloads.cn/android/repository/android-ndk-r27c-linux.zip
echo x android-ndk-r27c-linux
echo sudo mv android-ndk-r27c-linux /opt
echo export ANDROID_NDK=/opt/android-ndk-r27c

cmake -DCMAKE_BUILD_TYPE=relwithdebinfo -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cmake --build . --config relwithdebinfo -- -j$(nproc)

# popd
cd ..

# echo continue && read -n 1
