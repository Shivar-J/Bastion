#!/usr/bin/env bash

set -o pipefail

base="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
bastionDir="$base/Bastion"
buildDir="$bastionDir/build"
nativeDir="$base/Tenaille/native"
csproj="$base/Tenaille/Tenaille.csproj"

mkdir -p "$buildDir"

if [ ! -f "$buildDir/CMakeCache.txt" ]; then
    if command -v ninja >/dev/null 2>&1; then
        cmake -S "$base" -B "$buildDir" -G Ninja -DCMAKE_BUILD_TYPE=Debug
    else
        cmake -S "$base" -B "$buildDir" -DCMAKE_BUILD_TYPE=Debug
    fi
fi

cmake --build "$buildDir" --config Debug

dotnet build "$csproj" -c Debug
dotnet run --project "$csproj" -c Debug
