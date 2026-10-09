#!/bin/bash

set -euo pipefail

proj_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
tools_dir="$proj_dir/.tools"
version=12.1.4
archive="ghidra_${version}_PUBLIC_20260921.zip"

# Install dependencies.
sudo apt-get update
sudo apt-get install -y curl ca-certificates unzip openjdk-21-jdk

mkdir -p "$tools_dir/downloads"

# Download and extract the official release.
if [[ ! -d "$tools_dir/ghidra_${version}_PUBLIC" ]]; then
    curl -fL --retry 3 \
        "https://github.com/NationalSecurityAgency/ghidra/releases/download/Ghidra_${version}_build/$archive" \
        -o "$tools_dir/downloads/$archive"
    unzip -q "$tools_dir/downloads/$archive" -d "$tools_dir"
fi

# Give run.sh a stable installation path.
ln -sfn "ghidra_${version}_PUBLIC" "$tools_dir/ghidra"
