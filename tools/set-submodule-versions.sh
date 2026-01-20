#!/bin/bash
# ProLife Submodule Version Management Script
# Interactive script to set submodule versions

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Helper functions
print_header() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}========================================${NC}"
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

print_info() {
    echo -e "${BLUE}ℹ $1${NC}"
}

print_header "ProLife Submodule Version Management"
echo ""

cd "$REPO_ROOT"

# Check if we're in a git repository
if ! git rev-parse --is-inside-work-tree > /dev/null 2>&1; then
    print_error "Not in a git repository"
    exit 1
fi

# Initialize submodules if needed
echo "Ensuring submodules are initialized..."
git submodule update --init --recursive >/dev/null 2>&1
echo ""

# Count submodules
SUBMODULE_COUNT=$(git submodule status | wc -l)

echo "Found $SUBMODULE_COUNT submodules"
echo ""
echo "Select an option:"
echo "  1. Auto-update all submodules to latest tagged versions"
echo "  2. Interactively select versions for each submodule"
echo "  3. Set specific version for one submodule"
echo "  4. Show current submodule versions"
echo "  5. Exit"
echo ""

read -p "Enter your choice (1-5): " CHOICE

case $CHOICE in
    1)
        # Auto-update all
        echo ""
        print_header "Auto-Updating All Submodules"
        echo ""
        
        UPDATED_COUNT=0
        
        git submodule status | while read -r line; do
            commit=$(echo "$line" | awk '{print $1}' | sed 's/^[+-]//')
            path=$(echo "$line" | awk '{print $2}')
            
            if [ -d "$path" ]; then
                echo "Processing $path..."
                cd "$path"
                
                # Fetch latest tags
                git fetch --tags --quiet 2>/dev/null
                
                # Get latest semantic version tag
                latest_tag=$(git tag -l | grep -E "^v?[0-9]+\.[0-9]+\.[0-9]+$" | sort -V | tail -1)
                
                if [ -n "$latest_tag" ]; then
                    current_tag=$(git describe --tags --exact-match 2>/dev/null || echo "")
                    
                    if [ "$current_tag" = "$latest_tag" ]; then
                        print_info "Already on latest tag: $latest_tag"
                    else
                        print_info "Updating from $current_tag to $latest_tag"
                        if git checkout "$latest_tag" --quiet 2>/dev/null; then
                            UPDATED_COUNT=$((UPDATED_COUNT + 1))
                            cd "$REPO_ROOT"
                            git add "$path"
                            cd "$path"
                            print_success "Updated to $latest_tag"
                        else
                            print_error "Failed to checkout $latest_tag"
                        fi
                    fi
                else
                    print_warning "No semantic version tags found"
                fi
                
                cd "$REPO_ROOT"
                echo ""
            fi
        done
        
        echo ""
        print_header "Summary"
        echo "Updated $UPDATED_COUNT submodule(s)"
        echo ""
        echo "Review changes with: git status"
        echo "Commit changes with: git commit -m 'chore(deps): update submodules'"
        echo ""
        ;;
        
    2)
        # Interactive update
        echo ""
        print_header "Interactive Submodule Update"
        echo ""
        
        git submodule status | while read -r line; do
            commit=$(echo "$line" | awk '{print $1}' | sed 's/^[+-]//')
            path=$(echo "$line" | awk '{print $2}')
            name=$(basename "$path")
            
            if [ -d "$path" ]; then
                echo ""
                echo "----------------------------------------"
                echo "Submodule: $name"
                echo "Path: $path"
                echo "Current commit: ${commit:0:7}"
                echo ""
                
                cd "$path"
                
                # Get current tag/branch
                current_tag=$(git describe --tags --exact-match 2>/dev/null || echo "")
                if [ -n "$current_tag" ]; then
                    echo "Current tag: $current_tag"
                fi
                
                # Fetch tags
                echo "Fetching tags..."
                git fetch --tags --quiet 2>/dev/null
                
                echo ""
                echo "Available tags (last 10):"
                git tag -l | grep -E "^v?[0-9]+\.[0-9]+\.[0-9]+$" | sort -V | tail -10
                echo ""
                
                read -p "Enter version to use (or 'skip' to skip, 'latest' for latest tag): " version
                
                if [ "$version" = "skip" ]; then
                    print_info "Skipping $name"
                    cd "$REPO_ROOT"
                elif [ "$version" = "latest" ]; then
                    latest=$(git tag -l | grep -E "^v?[0-9]+\.[0-9]+\.[0-9]+$" | sort -V | tail -1)
                    if [ -n "$latest" ]; then
                        print_info "Checking out $latest..."
                        if git checkout "$latest" --quiet 2>/dev/null; then
                            print_success "Updated to $latest"
                            cd "$REPO_ROOT"
                            git add "$path"
                        else
                            print_error "Failed to checkout $latest"
                            cd "$REPO_ROOT"
                        fi
                    else
                        print_error "No tags found"
                        cd "$REPO_ROOT"
                    fi
                elif [ -n "$version" ]; then
                    print_info "Checking out $version..."
                    if git checkout "$version" --quiet 2>/dev/null; then
                        print_success "Updated to $version"
                        cd "$REPO_ROOT"
                        git add "$path"
                    else
                        print_error "Failed to checkout $version"
                        cd "$REPO_ROOT"
                    fi
                else
                    cd "$REPO_ROOT"
                fi
            fi
        done
        
        echo ""
        print_header "Interactive Update Complete"
        echo ""
        echo "Review changes with: git status"
        echo "Commit changes with: git commit -m 'chore(deps): update submodules'"
        echo ""
        ;;
        
    3)
        # Single submodule update
        echo ""
        print_header "Update Single Submodule"
        echo ""
        
        echo "Available submodules:"
        echo ""
        
        # Create indexed list
        declare -a SUBMODULES
        INDEX=0
        git submodule status | while read -r line; do
            path=$(echo "$line" | awk '{print $2}')
            INDEX=$((INDEX + 1))
            echo "  $INDEX. $path"
            SUBMODULES[$INDEX]="$path"
        done
        
        # Read submodules into array for selection
        readarray -t SUBMOD_ARRAY < <(git submodule status | awk '{print $2}')
        
        echo ""
        read -p "Enter submodule number: " SUBMOD_INDEX
        
        if [ -z "$SUBMOD_INDEX" ] || [ "$SUBMOD_INDEX" -lt 1 ] || [ "$SUBMOD_INDEX" -gt "${#SUBMOD_ARRAY[@]}" ]; then
            print_error "Invalid submodule number"
            exit 1
        fi
        
        SUBMOD_PATH="${SUBMOD_ARRAY[$((SUBMOD_INDEX - 1))]}"
        
        echo ""
        echo "Selected: $SUBMOD_PATH"
        echo ""
        
        if [ ! -d "$SUBMOD_PATH" ]; then
            print_error "Submodule not found: $SUBMOD_PATH"
            exit 1
        fi
        
        cd "$SUBMOD_PATH"
        
        echo "Fetching tags..."
        git fetch --tags --quiet 2>/dev/null
        
        echo ""
        echo "Available tags (last 20):"
        git tag -l | grep -E "^v?[0-9]+\.[0-9]+\.[0-9]+$" | sort -V | tail -20
        echo ""
        
        read -p "Enter version to checkout: " VERSION
        
        if [ -z "$VERSION" ]; then
            print_error "No version specified"
            exit 1
        fi
        
        print_info "Checking out $VERSION..."
        if git checkout "$VERSION" --quiet 2>/dev/null; then
            print_success "Updated $SUBMOD_PATH to $VERSION"
            cd "$REPO_ROOT"
            git add "$SUBMOD_PATH"
            echo ""
            echo "Review changes with: git status"
            echo "Commit changes with: git commit -m 'chore(deps): update $SUBMOD_PATH to $VERSION'"
        else
            print_error "Failed to checkout $VERSION"
            exit 1
        fi
        
        echo ""
        ;;
        
    4)
        # Show versions
        echo ""
        print_header "Current Submodule Versions"
        echo ""
        
        git submodule status | while read -r line; do
            commit=$(echo "$line" | awk '{print $1}' | sed 's/^[+-]//')
            path=$(echo "$line" | awk '{print $2}')
            name=$(basename "$path")
            
            echo "$name:"
            echo "  Path: $path"
            echo "  Commit: ${commit:0:7}"
            
            if [ -d "$path" ]; then
                cd "$path"
                
                # Get tag if on a tag
                tag=$(git describe --tags --exact-match 2>/dev/null || echo "")
                if [ -n "$tag" ]; then
                    echo "  Tag: $tag"
                fi
                
                # Get branch
                branch=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "")
                if [ -n "$branch" ]; then
                    echo "  Branch: $branch"
                fi
                
                cd "$REPO_ROOT"
            fi
            echo ""
        done
        ;;
        
    5)
        echo "Exiting..."
        exit 0
        ;;
        
    *)
        print_error "Invalid choice"
        exit 1
        ;;
esac
