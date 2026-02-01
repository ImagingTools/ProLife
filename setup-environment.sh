#!/bin/bash
# setup-environment.sh
# This script initializes the ProLife build environment by configuring environment variables.
# Note: Dependencies must be cloned and built separately. See BUILDING.md for details.

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

# Check if required dependencies are available
echo ""
echo "Checking for required dependencies..."
echo "Note: All dependencies must be cloned and available externally."
echo ""

# Set environment variables
# These should point to your externally managed dependency repositories
# You can customize these paths before sourcing this script

if [ -z "$IMTCOREDIR" ]; then
    echo "WARNING: IMTCOREDIR is not set"
fi

if [ -z "$PUMADIR" ]; then
    echo "WARNING: PUMADIR is not set"
fi

if [ -z "$LISADIR" ]; then
    echo "WARNING: LISADIR is not set"
fi

if [ -z "$ACFDIR" ]; then
    echo "WARNING: ACFDIR is not set"
fi

if [ -z "$ACFSLNDIR" ]; then
    echo "WARNING: ACFSLNDIR is not set"
fi

if [ -z "$AGENTINODIR" ]; then
    echo "WARNING: AGENTINODIR is not set"
fi

export PROLIFEDIR="${PROJECT_ROOT}"

echo ""
echo "Environment variables configured:"
echo "  IMTCOREDIR=${IMTCOREDIR:-NOT SET}"
echo "  PUMADIR=${PUMADIR:-NOT SET}"
echo "  LISADIR=${LISADIR:-NOT SET}"
echo "  ACFDIR=${ACFDIR:-NOT SET}"
echo "  ACFSLNDIR=${ACFSLNDIR:-NOT SET}"
echo "  AGENTINODIR=${AGENTINODIR:-NOT SET}"
echo "  PROLIFEDIR=${PROLIFEDIR}"

echo ""
echo "=========================================="
echo "Setup complete!"
echo "=========================================="
echo ""
echo "IMPORTANT: Before building, ensure all dependency directories are set:"
echo "  export IMTCOREDIR=/path/to/ImtCore"
echo "  export PUMADIR=/path/to/Puma"
echo "  export LISADIR=/path/to/Lisa"
echo "  export ACFDIR=/path/to/Acf"
echo "  export ACFSLNDIR=/path/to/AcfSln"
echo "  export AGENTINODIR=/path/to/Agentino"
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
