@ECHO OFF
PUSHD build
cmake .. -DCMAKE_TOOLCHAIN_FILE=D:/vcpkg/scripts/buildsystems/vcpkg.cmake
POPD
cmake --build build
@ECHO ON