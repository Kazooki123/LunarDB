#!/bin/bash
set -e

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Print colored message
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check for required dependencies
check_dependencies() {
    local deps=("curl" "jq")
    for dep in "${deps[@]}"; do
        if ! command -v "$dep" &> /dev/null; then
            log_error "$dep is required but not installed."
            exit 1
        fi
    done
}

# Detect OS
detect_os() {
    local os=$(uname | tr '[:upper:]' '[:lower:]')
    case "$os" in
        "linux")
            if [ -f "/etc/arch-release" ]; then
                echo "arch"
            else
                echo "ubuntu"
            fi
            ;;
        "windows" | "mingw"* | "cygwin"*)
            echo "windows"
            ;;
        *)
            log_error "Unsupported operating system: $os"
            log_error "LunarDB is only available for Arch Linux, Ubuntu, and Windows."
            exit 1
            ;;
    esac
}

# Download file with progress bar
download_file() {
    local url="$1"
    local output="$2"

    # Create parent directories if they don't exist
    mkdir -p "$(dirname "$output")"

    if ! curl -L --progress-bar -o "$output" "$url"; then
        log_error "Failed to download $url"
        return 1
    fi
}

# Update additional files
update_files() {
    local repo_url="$1"
    local file="$2"

    # Check if the file is in excluded directories
    while IFS= read -r excluded_dir; do
        if [[ "$file" == "$excluded_dir"* ]]; then
            log_warning "Skipping excluded directory: $file"
            return 0
        fi
    done < <(jq -r '.excluded_dirs[]' "$MANIFEST_PATH")

    log_info "Updating: $file"
}

# Main update function
main() {
    log_info "Starting LunarDB update process..."

    # Check dependencies
    check_dependencies

    # Configuration
    REPO_URL="https://github.com/Kazooki123/LunarDB/releases/latest/download"
    MANIFEST_PATH="manifest.json"

    # Check if manifest exists
    if [ ! -f "$MANIFEST_PATH" ]; then
        log_error "manifest.json not found"
        exit 1
    fi

    # Detect OS
    OS=$(detect_os)
    log_info "Detected OS: $OS"

    # Get versions
    log_info "Checking versions..."
    LATEST_VERSION=$(curl -s "$REPO_URL/version.txt")
    if [ -z "$LATEST_VERSION" ]; then
        log_error "Failed to fetch latest version"
        exit 1
    fi

    CURRENT_VERSION=$(jq -r '.version' "$MANIFEST_PATH")

    log_info "Current version: $CURRENT_VERSION"
    log_info "Latest version: $LATEST_VERSION"

    # Check if update is needed
    if [ "$CURRENT_VERSION" = "$LATEST_VERSION" ]; then
        log_info "LunarDB is already up-to-date"
        exit 0
    fi

    # Get binary path from manifest
    BINARY_PATH=$(jq -r ".binaries[] | select(.os==\"$OS\") | .path" "$MANIFEST_PATH")

    if [ -z "$BINARY_PATH" ]; then
        log_error "No binary found for $OS in manifest.json"
        exit 1
    fi

    # Create backup of current binary if it exists
    if [ -f "$BINARY_PATH" ]; then
        log_info "Creating backup of current binary..."
        cp "$BINARY_PATH" "${BINARY_PATH}.backup"
    fi

    # Download and update binary
    log_info "Downloading new binary..."
    download_file "$REPO_URL/$BINARY_PATH" "$BINARY_PATH"

    # Make binary executable
    chmod +x "$BINARY_PATH"

    # Update additional files
    log_info "Updating additional files..."
    while IFS= read -r file; do
        if [ ! -z "$file" ]; then
            update_files "$REPO_URL" "$file"
        fi
    done < <(jq -r '.files_to_update[]' "$MANIFEST_PATH")

    log_info "Update process completed successfully!"
}

main "$@"
