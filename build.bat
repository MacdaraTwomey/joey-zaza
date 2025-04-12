
pushd build
clang++ -o ProximityUxhostl.exe -O3 ..\\src\\ProximityUxhost.cpp -l Ole32.lib -l Shell32.lib
popd