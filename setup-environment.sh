#!/bin/bash
# setup-environment.sh
# This script initializes the ProLife build environment by setting up submodules
# and configuring environment variables.

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}" && pwd)"

echo "=========================================="
echo "ProLife Environment Setup"
echo "=========================================="
echo ""

# Check Git version
GIT_VERSION=$(git --version | sed -E 's/.*([0-9]+\.[0-9]+\.[0-9]+).*/\1/')
echo "Git version: $GIT_VERSION"

# Initialize submodules
echo ""
echo "Initializing Git submodules..."
echo "This may take a few minutes on first run."
echo ""

if git submodule update --init --recursive; then
    echo "[OK] Submodules initialized successfully"
else
    echo "[ERROR] Failed to initialize submodules"
    echo ""
    echo "Some submodules may be private and require authentication."
    echo "Please ensure you have:"
    echo "  - SSH keys configured and added to GitHub"
    echo "  - Or Personal Access Token with appropriate permissions"
    echo "  - Repository access granted by the ImagingTools organization"
    echo ""
    echo "You can configure Git to use SSH URLs globally:"
    echo '  git config --global url."git@github.com:".insteadOf "https://github.com/"'
    echo ""
    exit 1
fi

# Show submodule status
echo ""
echo "Submodule status:"
git submodule status

# Set environment variables pointing to submodules
export IMTCOREDIR="${PROJECT_ROOT}/3rdParty/ImtCore"
export PUMADIR="${PROJECT_ROOT}/3rdParty/Puma"
export LISADIR="${PROJECT_ROOT}/3rdParty/Lisa"
export ACFDIR="${PROJECT_ROOT}/3rdParty/Acf"
export ACFSLNDIR="${PROJECT_ROOT}/3rdParty/AcfSln"
export AGENTINODIR="${PROJECT_ROOT}/3rdParty/Agentino"
export PROLIFEDIR="${PROJECT_ROOT}"

echo ""
echo "Environment variables configured:"
echo "  IMTCOREDIR=${IMTCOREDIR}"
echo "  PUMADIR=${PUMADIR}"
echo "  LISADIR=${LISADIR}"
echo "  ACFDIR=${ACFDIR}"
echo "  ACFSLNDIR=${ACFSLNDIR}"
echo "  AGENTINODIR=${AGENTINODIR}"
echo "  PROLIFEDIR=${PROLIFEDIR}"

echo ""
echo "=========================================="
echo "Setup complete!"
echo "=========================================="
echo ""
echo "To build ProLife:"
echo "  cd Build/CMake"
echo "  mkdir -p build && cd build"
echo "  cmake .."
echo "  cmake --build . --config Release"
echo ""
echo "Or source this script to set environment variables:"
echo "  source ./setup-environment.sh"
echo ""
