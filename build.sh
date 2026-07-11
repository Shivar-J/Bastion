#!/bin/bash

clean_flag=false
debug_flag=false
release_flag=false
mode="Debug"

print_usage() {
  printf "Usage:
  [-d debug]
  [-r release]

  [-c clean]
  "
}

invalid_usage() {
  printf "Flags %s and %s cannot be set together\n" "$1" "$2"
  exit 1
}

while getopts ":cdr" flag; do
  case "${flag}" in
    c) clean_flag=true;;
    d) debug_flag=true;;
    r) release_flag=true;;
    *) print_usage
      exit 1;;
  esac
done

if [ "$debug_flag" = true ] && [ "$release_flag" = true ]; then
  invalid_usage "-d" "-r"
fi

if [ "$release_flag" = true ]; then
  mode="Release"
fi

base="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
bastionDir="$base/Bastion"
buildDir="$bastionDir/build"
nativeDir="$base/Tenaille/native"
csproj="$base/Tenaille/Tenaille.csproj"

mkdir -p "$buildDir"
mkdir -p "$nativeDir"

rebuild_build_bastion() {
  printf "Rebuild %s\n" "$1"
  rm -rf "$buildDir"
  rm -rf "$nativeDir"

  mkdir -p "$buildDir"
  mkdir -p "$nativeDir"

  build_bastion "$1"
}

build_bastion() {
  mkdir -p "$buildDir/$1"

  if [ ! -e "$buildDir/CMakeCache.txt" ]; then
    cmake -S "$base" -B "$buildDir/$1" -G Ninja -DCMAKE_BUILD_TYPE="$1"
  else
    cmake -S "$base" -B "$buildDir/$1" -DCMAKE_BUILD_TYPE="$1"
  fi

  slangc "$bastionDir/shaders/shader.slang" -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name -entry vertMain -entry fragMain -o "$bastionDir/shaders/slang.spv"

  cmake --build "$buildDir/$1" --config "$1"

  cp -u "$buildDir/$1/shaders/"*.spv "$nativeDir/"
  cp -u "$buildDir/$1/libBastion.so" "$nativeDir/"

  dotnet build "$csproj" -c "$1" --no-restore
  dotnet run --project "$csproj" -c "$1" --no-restore
}

if [ "$clean_flag" = true ]; then
  rebuild_build_bastion "$mode"
else
  build_bastion "$mode"
fi