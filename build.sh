#!/bin/bash

# Usage: 
# build.sh win      
# build.sh linux [win-path]
# build.sh msvc

# ------------ Argument handling -------------------------------------------------
ArgCount=$#
Root=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

OS=$1
if [[ $ArgCount == 2 ]]; then LinuxOutputPathType=$2; fi

# ------------ Flags and sources -------------------------------------------------
Source="$Root/src/app.cpp"
Include="-I $Root/deps -I $Root/zydis-amalgated"   # -isystem $Root/zydis/include -isystem $Root/zydis/dependencies/zycore/include
CommonFlags="-DBASE_DEBUG=1"
Executable="dedbg"

ClangFlags="-g -std=c++20"
ClangFlags+=" -fsanitize=address -fuse-ld=lld"
ClangFlags+=" -Wall -pedantic-errors -Wno-unused-variable -Wno-gnu-anonymous-struct -Wno-writable-strings"
ClangFlags+=" -Wno-nested-anon-types -Wno-gnu-zero-variadic-macro-arguments -Wno-missing-braces"
ClangFlags+=" -fno-strict-aliasing -Wno-unused-function -Wno-language-extension-token"
#ClangFlags+=" -fdiagnostics-color"    # forces colour even when piping to sed

ClFlags="-std:c++latest /Zc:preprocessor -Zc:strictStrings- -W3 -wd5105 -wd4201 -wd4505 -INCREMENTAL:NO -FC -EHs- -nologo -Zi"

# ------------ Boilerplate -------------------------------------------------------
if [[ $OS == "win" ]];   then Compiler="clang++.exe" Out="-o "; fi
if [[ $OS == "linux" ]]; then Compiler="clang++"     Out="-o "; fi
if [[ $OS == "msvc" ]];  then Compiler="cl.exe"      Out="-out:"; fi

function wsl_to_win() { echo "$1" | sed 's/\/mnt\/c/c:/g' ; }

if [[ $OS == "win" ]] || [[ $OS == "msvc" ]]; then Source=`wsl_to_win "$Source"`; fi
if [[ $OS == "win" ]] || [[ $OS == "msvc" ]]; then Include=`wsl_to_win "$Include"`; fi

# Link libdwarf
# Beacuse we compiled libdwarf with zstd decompression library enabled we need z and zstd
if [[ $OS == "linux" ]];  then Link="-l ncursesw -L /usr/local/lib -l dwarf -L $Root/zydis-amalgated -l zydis -lz -lzstd"; fi

Flags="$CommonFlags"
if [[ $OS == "linux" ]] || [[ $OS == "win" ]]; then Flags+=" $ClangFlags"; fi
if [[ $OS == "msvc" ]]; 					   then Flags+=" $ClFlags"; fi

# ------------ Compile -----------------------------------------------------------
mkdir -p "$Root/build"
pushd "$Root/build" > /dev/null

CMD="bear -- $Compiler $Source $Include $Flags $Link $Out$Executable"
echo "$CMD"

if [[ $LinuxOutputPathType == "win-path" ]]
then
	# Fixup error paths so we can jump to error on WSL
	OUTPUT=`eval "$CMD" 2>&1`
	wsl_to_win "$OUTPUT"
else
	eval "$CMD"
fi

cp $Root/zydis-amalgated/libzydis.so .

popd > /dev/null
