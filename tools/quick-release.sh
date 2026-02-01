#!/bin/bash
# Quick Release Preparation Script
# Automates the most common release preparation steps

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

print_step() {
    echo -e "${BLUE}$1${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

# Check if version is provided
if [ -z "$1" ]; then
    print_error "Version is required"
    echo "Usage: $0 VERSION [--skip-tests]"
    echo "Example: $0 2.1.0"
    echo ""
    echo "Options:"
    echo "  --skip-tests    Skip build and test steps"
    exit 1
fi

VERSION="$1"
SKIP_TESTS=false

if [ "$2" = "--skip-tests" ]; then
    SKIP_TESTS=true
fi

# Validate version format
if ! [[ $VERSION =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    print_error "Invalid version format. Use MAJOR.MINOR.PATCH (e.g., 2.1.0)"
    exit 1
fi

echo ""
echo "========================================="
echo "🚀 ProLife Quick Release Preparation"
echo "========================================="
echo ""
echo "Version: v$VERSION"
echo "Date: $(date '+%Y-%m-%d')"
echo ""

cd "$REPO_ROOT"

# Step 1: Initial validation
print_step "1️⃣ Checking current state..."
echo ""

if ! git diff-index --quiet HEAD --; then
    print_warning "You have uncommitted changes. Please commit or stash them first."
    git status --short
    echo ""
    read -p "Continue anyway? (y/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

print_success "Repository state checked"
echo ""

# Step 2: Create release branch
print_step "2️⃣ Creating release branch..."
echo ""

branch_name="release/v$VERSION"

if git rev-parse --verify "$branch_name" >/dev/null 2>&1; then
    print_warning "Branch $branch_name already exists"
    read -p "Switch to existing branch? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        git checkout "$branch_name"
    else
        print_error "Aborting"
        exit 1
    fi
else
    git checkout -b "$branch_name"
    print_success "Created and switched to branch: $branch_name"
fi

echo ""

# Step 3: Initialize submodules if needed
print_step "3️⃣ Ensuring submodules are initialized..."
echo ""

git submodule update --init --recursive
print_success "Submodules initialized"
echo ""

# Step 4: Auto-update submodules
print_step "4️⃣ Updating submodules to latest tagged versions..."
echo ""

"$REPO_ROOT/tools/validate-release.sh" auto-update

echo ""

# Step 5: Build and test (if not skipped)
if [ "$SKIP_TESTS" = false ]; then
    print_step "5️⃣ Building and testing..."
    echo ""
    
    BUILD_DIR="$REPO_ROOT/build-release-check"
    
    if [ -d "$BUILD_DIR" ]; then
        print_warning "Removing existing build directory..."
        rm -rf "$BUILD_DIR"
    fi
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    print_warning "Configuring build..."
    if ! cmake ../Build/CMake -DCMAKE_BUILD_TYPE=Release; then
        print_error "CMake configuration failed"
        cd "$REPO_ROOT"
        exit 1
    fi
    
    print_warning "Building..."
    if ! cmake --build . --config Release; then
        print_error "Build failed"
        cd "$REPO_ROOT"
        exit 1
    fi
    
    print_success "Build successful"
    
    print_warning "Running tests..."
    if ! ctest --config Release --output-on-failure; then
        print_warning "Some tests failed. Please review."
        read -p "Continue with release preparation? (y/n) " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            cd "$REPO_ROOT"
            exit 1
        fi
    else
        print_success "All tests passed"
    fi
    
    cd "$REPO_ROOT"
    echo ""
else
    print_warning "5️⃣ Skipping build and test (--skip-tests flag)"
    echo ""
fi

# Step 6: Update version in CMakeLists.txt
print_step "6️⃣ Updating version in CMakeLists.txt..."
echo ""

CMAKE_FILE="$REPO_ROOT/Build/CMake/CMakeLists.txt"
if [ -f "$CMAKE_FILE" ]; then
    sed -i "s/project(ProLifeAll VERSION [0-9.]*)/project(ProLifeAll VERSION $VERSION)/" "$CMAKE_FILE"
    print_success "Updated CMakeLists.txt to version $VERSION"
else
    print_warning "CMakeLists.txt not found at expected location"
fi

echo ""

# Step 7: Generate submodule report
print_step "7️⃣ Generating submodule version report..."
echo ""

REPORT_FILE="$REPO_ROOT/RELEASE_SUBMODULES_v$VERSION.md"
"$REPO_ROOT/tools/validate-release.sh" report "$REPORT_FILE"

echo ""

# Step 8: Pin submodules
print_step "8️⃣ Pinning submodules to current commits..."
echo ""

"$REPO_ROOT/prepare-release.sh" --pin-submodules
print_success "Submodules pinned"

echo ""

# Step 9: Final validation
print_step "9️⃣ Running final validation..."
echo ""

if "$REPO_ROOT/tools/validate-release.sh" validate "v$VERSION"; then
    print_success "Validation passed"
else
    print_warning "Validation found issues. Please review before proceeding."
fi

echo ""

# Step 10: Summary
echo "========================================="
echo "✅ Automated preparation complete!"
echo "========================================="
echo ""
echo "Summary of changes:"
git status --short
echo ""
echo "Next steps (MANUAL):"
echo ""
echo "  1. Review and update CHANGELOG.md with release notes:"
echo "     vim CHANGELOG.md"
echo ""
echo "  2. Review the submodule version report:"
echo "     cat $REPORT_FILE"
echo ""
echo "  3. Review all changes:"
echo "     git diff"
echo ""
echo "  4. If everything looks good, commit the changes:"
echo "     git add ."
echo "     git commit -m \"chore: prepare release v$VERSION"
echo ""
echo "     - Updated submodules to stable versions"
echo "     - Updated CHANGELOG.md"
echo "     - Updated version in CMakeLists.txt"
echo "     - Pinned all dependencies\""
echo ""
echo "  5. Push the release branch:"
echo "     git push -u origin $branch_name"
echo ""
echo "  6. Create a Pull Request for review"
echo ""
echo "  7. After PR approval, run the GitHub Actions workflow:"
echo "     - Go to Actions → Release Preparation"
echo "     - Run with branch: $branch_name"
echo "     - Target tag: v$VERSION"
echo "     - Base tag: (previous version)"
echo "     - Dry run: false"
echo ""
echo "Files modified:"
echo "  - Build/CMake/CMakeLists.txt (version updated)"
echo "  - RELEASE_SUBMODULES_v$VERSION.md (created)"
echo "  - All submodule references (pinned to specific commits)"
echo ""
