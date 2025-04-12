
mkdir -p build
cd build
cl.exe -std:c++latest /Zc:preprocessor -Zc:strictStrings- -O2 -W3 -wd5105 -wd4201 -wd4996 -wd4505 -INCREMENTAL:NO -FC -EHs- -nologo \
	-Zi -fsanitize=address  ..\\src\\zaza.cpp /link Ole32.lib Shell32.lib /INFERASANLIBS