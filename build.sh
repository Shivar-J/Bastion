#!/usr/bin/env bash

set -o pipefail

read -p "Build Mode
1: Debug
2: Release
3: Delete Debug & Release Files
" mode

base="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
bastionDir="$base/Bastion"
buildDir="$bastionDir/build"
nativeDir="$base/Tenaille/native"
csproj="$base/Tenaille/Tenaille.csproj"

mkdir -p "$buildDir"
mkdir -p "$nativeDir"

if [ "$mode" == "1" ]; then
  mkdir -p "$buildDir/Debug"

  if [ ! -a "$buildDir/CMakeCache.txt" ]; then
    cmake -S "$base" -B "$buildDir/Debug" -G Ninja -DCMAKE_BUILD_TYPE=Debug
  else
    cmake -S "$base" -B "$buildDir/Debug" -DCMAKE_BUILD_TYPE=Debug
  fi

  slangc "$bastionDir/shaders/shader.slang" -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name -entry vertMain -entry fragMain -o "$bastionDir/shaders/slang.spv"

  cmake --build "$buildDir/Debug" --config Debug

  cp -u "$buildDir/Debug/shaders/"*.spv "$nativeDir/"
  cp -u "$buildDir/Debug/libBastion.so" "$nativeDir/"

  dotnet build "$csproj" -c Debug --no-restore
  dotnet run --project "$csproj" -c Debug --no-restore

elif [ "$mode" == "2" ]; then
  mkdir -p "$buildDir/Release"

  if [ ! -a "$buildDir/CMakeCache.txt" ]; then
    cmake -S "$base" -B "$buildDir/Release" -G Ninja -DCMAKE_BUILD_TYPE=Release
  else
    cmake -S "$base" -B "$buildDir/Release" -DCMAKE_BUILD_TYPE=Release
  fi

  slangc "$bastionDir/shaders/shader.slang" -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name -entry vertMain -entry fragMain -o "$bastionDir/shaders/slang.spv"

  cmake --build "$buildDir/Release" --config Release

  cp -u "$buildDir/Release/shaders/"*.spv "$nativeDir/"
  cp -u "$buildDir/Release/libBastion.so" "$nativeDir/"

  dotnet build "$csproj" -c Release --no-restore
  dotnet run --project "$csproj" -c Release --no-restore
elif [ "$mode" == "3" ]; then
  rm -rf "$buildDir"
  rm -rf "$nativeDir"
else
  echo "Unknown parameter"
  exit 1
fi