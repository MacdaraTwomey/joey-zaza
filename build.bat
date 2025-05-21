
pushd build
clang++ -o ProximityUxhostl.exe -mwindows -O3 ..\\src\\ProximityUxhost.cpp -l Ole32.lib -l Shell32.lib 
popd