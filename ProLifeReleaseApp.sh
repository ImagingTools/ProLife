#!/bin/bash
# ProLife Release App Launcher
# This script launches the ProLife Release App

echo "Starting ProLife Release App..."

# Check if Python is available
if ! command -v python3 &> /dev/null; then
    echo "ERROR: Python 3 is not installed or not in PATH"
    echo "Please install Python 3.7 or later"
    exit 1
fi

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Launch the app
python3 "$SCRIPT_DIR/ProLifeReleaseApp.py"

if [ $? -ne 0 ]; then
    echo ""
    echo "ERROR: Failed to launch ProLife Release App"
    read -p "Press Enter to continue..."
fi
