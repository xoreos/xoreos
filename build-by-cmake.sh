#!/usr/bin/env bash
#
# xoreos - macOS (Homebrew) CMake build helper.
#
# Configures and builds xoreos out-of-tree in ./build.
# Records the PKG_CONFIG_PATH needed for Homebrew keg-only deps
# (openal-soft, libxml2, xz, zlib) so CMake can find them.
#
# Usage:
#   ./build-by-cmake.sh                # configure + build (Debug)
#   BUILD_TYPE=Release ./build-by-cmake.sh
#   ./build-by-cmake.sh clean          # remove the build dir first

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
BUILD_TYPE="${BUILD_TYPE:-Debug}"
JOBS="${JOBS:-$(sysctl -n hw.ncpu)}"

# Homebrew keg-only dependencies aren't on the default pkg-config search path.
BREW_PREFIX="$(brew --prefix)"
export PKG_CONFIG_PATH="\
${BREW_PREFIX}/opt/openal-soft/lib/pkgconfig:\
${BREW_PREFIX}/opt/libxml2/lib/pkgconfig:\
${BREW_PREFIX}/opt/zlib-ng-compat/lib/pkgconfig:\
${BREW_PREFIX}/opt/xz/lib/pkgconfig:\
${PKG_CONFIG_PATH:-}"

if [ "${1:-}" = "clean" ]; then
  echo ">> Removing ${BUILD_DIR}"
  rm -rf "${BUILD_DIR}"
fi

echo ">> Configuring (${BUILD_TYPE}) in ${BUILD_DIR}"
cmake -S "${SCRIPT_DIR}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"

echo ">> Building with ${JOBS} jobs"
cmake --build "${BUILD_DIR}" -j"${JOBS}"

echo ">> Done: ${BUILD_DIR}/bin/xoreos"
