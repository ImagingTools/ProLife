#!/bin/bash
# ProLife Release App - Setup Script
# This script checks and installs required dependencies

echo "========================================="
echo "ProLife Release App - Setup"
echo "========================================="
echo ""

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check Python
echo "Checking Python installation..."
if command -v python3 &> /dev/null; then
    PYTHON_VERSION=$(python3 --version 2>&1 | awk '{print $2}')
    echo -e "${GREEN}✓${NC} Python 3 is installed: $PYTHON_VERSION"
else
    echo -e "${RED}✗${NC} Python 3 is not installed"
    echo "  Please install Python 3.7 or later from https://www.python.org/"
    exit 1
fi

# Check Git
echo "Checking Git installation..."
if command -v git &> /dev/null; then
    GIT_VERSION=$(git --version | awk '{print $3}')
    echo -e "${GREEN}✓${NC} Git is installed: $GIT_VERSION"
else
    echo -e "${RED}✗${NC} Git is not installed"
    echo "  Please install Git from https://git-scm.com/"
    exit 1
fi

# Check CMake
echo "Checking CMake installation..."
if command -v cmake &> /dev/null; then
    CMAKE_VERSION=$(cmake --version | head -n1 | awk '{print $3}')
    echo -e "${GREEN}✓${NC} CMake is installed: $CMAKE_VERSION"
else
    echo -e "${YELLOW}⚠${NC} CMake is not installed (required for building)"
    echo "  Install CMake from https://cmake.org/ to use build features"
fi

# Check Tkinter (Python GUI library)
echo "Checking Tkinter availability..."
if python3 -c "import tkinter" 2>/dev/null; then
    echo -e "${GREEN}✓${NC} Tkinter is available"
else
    echo -e "${RED}✗${NC} Tkinter is not available"
    echo "  Installing Tkinter..."
    
    # Try to install based on OS
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command -v apt-get &> /dev/null; then
            echo "  Using apt-get to install python3-tk..."
            sudo apt-get update
            sudo apt-get install -y python3-tk
        elif command -v yum &> /dev/null; then
            echo "  Using yum to install python3-tkinter..."
            sudo yum install -y python3-tkinter
        elif command -v dnf &> /dev/null; then
            echo "  Using dnf to install python3-tkinter..."
            sudo dnf install -y python3-tkinter
        else
            echo -e "${RED}✗${NC} Could not install Tkinter automatically"
            echo "  Please install python3-tk or python3-tkinter for your distribution"
            exit 1
        fi
        
        # Verify installation
        if python3 -c "import tkinter" 2>/dev/null; then
            echo -e "${GREEN}✓${NC} Tkinter installed successfully"
        else
            echo -e "${RED}✗${NC} Failed to install Tkinter"
            exit 1
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "  Tkinter should be included with Python on macOS"
        echo "  If not working, try: brew install python-tk"
    fi
fi

echo ""
echo "========================================="
echo -e "${GREEN}Setup complete!${NC}"
echo "========================================="
echo ""
echo "You can now run the ProLife Release App:"
echo ""
echo "  ./ProLifeReleaseApp.sh"
echo ""
echo "Or directly with Python:"
echo ""
echo "  python3 ProLifeReleaseApp.py"
echo ""
