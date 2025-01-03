#!/bin/bash

echo "Starting update process..."

# Detect OS
OS=$(uname | tr '[:upper:]' '[:lower:]')
if [[ "$OS" == "linux" ]]; then
  if [[ -f "/etc/arch-release" ]]; then
    OS="arch"
  else
    OS="ubuntu"
  fi
elif [[ "$OS" == "mingw"* || "$OS" == "cygwin"* ]]; then
  OS="windows"
fi

REPO_URL="https://github.com/Kazooki123/LunarDB/releases/latest/download"
MANIFEST_PATH="manifest.json"
VERSION_PATH="version.txt"
LATEST_VERSION=$(curl -s $VERSION_PATH)

CURRENT_VERSION=$(jq -r '.version' $MANIFEST_PATH)

if [[ "$CURRENT_VERSION" == "$LATEST_VERSION" ]]; then
  echo "LunarDB is already up-to-date."
  exit 0
fi

echo "Updating to version $LATEST_VERSION..."

# UPDATE BINARIES
BINARY_PATH=$(jq -r ".binaries[] | select(.os==\"$OS\") | .path" $MANIFEST_PATH)

if [[ -z "$BINARY_PATH" ]]; then
  echo "No binary found for $OS in manifest.json."
  exit 1
fi

curl -L -o $BINARY_PATH "$REPO_URL/$BINARY_PATH"
chmod +x $BINARY_PATH

echo "Binary updated: $BINARY_PATH"

# Updating other files
echo "Updating other files.."
jq -r '.files_to_update[]' $MANIFEST_PATH | while read -r dir; dp
  echo "Updating: $dir"
done

echo "Update process completed!"
