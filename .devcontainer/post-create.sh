#!/usr/bin/env bash
set -euo pipefail

mkdir -p \
  "${VCPKG_DOWNLOADS:-/usr/local/vcpkg-downloads}" \
  "${VCPKG_DEFAULT_BINARY_CACHE:-/var/cache/vcpkg}" \
  "${SCCACHE_DIR:-/var/cache/sccache}"

sudo chown -R vscode:vscode \
  "${VCPKG_DOWNLOADS:-/usr/local/vcpkg-downloads}" \
  "${VCPKG_DEFAULT_BINARY_CACHE:-/var/cache/vcpkg}" \
  "${SCCACHE_DIR:-/var/cache/sccache}"

echo "[ok] cache directories ready"
echo "  VCPKG_DOWNLOADS=${VCPKG_DOWNLOADS:-/usr/local/vcpkg-downloads}"
echo "  VCPKG_DEFAULT_BINARY_CACHE=${VCPKG_DEFAULT_BINARY_CACHE:-/var/cache/vcpkg}"
echo "  SCCACHE_DIR=${SCCACHE_DIR:-/var/cache/sccache}"

echo "[ok] tool check"
command -v cmake >/dev/null 2>&1 && cmake --version | head -n 1 || true
command -v ninja >/dev/null 2>&1 && ninja --version || true
command -v clangd >/dev/null 2>&1 && clangd --version | head -n 1 || true
command -v sccache >/dev/null 2>&1 && sccache --version || true