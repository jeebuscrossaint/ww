#!/usr/bin/env bash
# Generate Wayland protocol C bindings from XML files

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROTOCOLS_DIR="${SCRIPT_DIR}/protocols"
BUILD_DIR="${SCRIPT_DIR}/build/protocols"

# Check if wayland-scanner is available
if ! command -v wayland-scanner &> /dev/null; then
    echo "Error: wayland-scanner not found!"
    echo "Please install wayland-protocols package:"
    echo "  Arch Linux:    sudo pacman -S wayland-protocols"
    echo "  Ubuntu/Debian: sudo apt install wayland-protocols"
    echo "  Fedora:        sudo dnf install wayland-protocols-devel"
    exit 1
fi

# Create output directory if it doesn't exist
mkdir -p "${BUILD_DIR}"

echo "Generating Wayland protocol bindings..."
echo "==========================================="

# Generate wlr-layer-shell protocol
echo "Generating wlr-layer-shell-unstable-v1..."
wayland-scanner client-header \
    "${PROTOCOLS_DIR}/wlr-layer-shell-unstable-v1.xml" \
    "${BUILD_DIR}/wlr-layer-shell-unstable-v1-client-protocol.h"

wayland-scanner private-code \
    "${PROTOCOLS_DIR}/wlr-layer-shell-unstable-v1.xml" \
    "${BUILD_DIR}/wlr-layer-shell-unstable-v1-protocol.c"

# Generate xdg-shell protocol
echo "Generating xdg-shell..."
wayland-scanner client-header \
    "${PROTOCOLS_DIR}/xdg-shell.xml" \
    "${BUILD_DIR}/xdg-shell-client-protocol.h"

wayland-scanner private-code \
    "${PROTOCOLS_DIR}/xdg-shell.xml" \
    "${BUILD_DIR}/xdg-shell-protocol.c"

# Fix C++ keyword collision in generated headers
# The wlr-layer-shell protocol uses 'namespace' as a parameter name,
# which is a reserved keyword in C++. We need to wrap inline functions
# with proper guards or use public-code instead of private-code.
echo ""
echo "Fixing C++ compatibility..."

# For the layer shell header, we need to handle the 'namespace' parameter
# The easiest fix is to use sed to rename it in the generated header
if [[ -f "${BUILD_DIR}/wlr-layer-shell-unstable-v1-client-protocol.h" ]]; then
    sed -i 's/const char \*namespace)/const char *name_space)/g' \
        "${BUILD_DIR}/wlr-layer-shell-unstable-v1-client-protocol.h"
    sed -i 's/, namespace)/, name_space)/g' \
        "${BUILD_DIR}/wlr-layer-shell-unstable-v1-client-protocol.h"
fi

# Also fix the .c file
if [[ -f "${BUILD_DIR}/wlr-layer-shell-unstable-v1-protocol.c" ]]; then
    sed -i 's/const char \*namespace)/const char *name_space)/g' \
        "${BUILD_DIR}/wlr-layer-shell-unstable-v1-protocol.c"
    sed -i 's/, namespace)/, name_space)/g' \
        "${BUILD_DIR}/wlr-layer-shell-unstable-v1-protocol.c"
fi

echo ""
echo "✓ Protocol bindings generated successfully!"
echo ""
echo "Generated files:"
echo "  ${BUILD_DIR}/wlr-layer-shell-unstable-v1-client-protocol.h"
echo "  ${BUILD_DIR}/wlr-layer-shell-unstable-v1-protocol.c"
echo "  ${BUILD_DIR}/xdg-shell-client-protocol.h"
echo "  ${BUILD_DIR}/xdg-shell-protocol.c"
echo ""
echo "Note: 'namespace' parameter renamed to 'name_space' for C++ compatibility"
echo ""
echo "You can now run: xmake"
