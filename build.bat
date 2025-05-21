
pushd build
clang++ -o ProximityUxhostl.exe -mwindows ..\\src\\ProximityUxhost.cpp -l Ole32.lib -l Shell32.lib 
popd