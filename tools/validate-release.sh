#!/bin/bash
# ProLife Release Validation and Automation Script
# This script validates and automates the release preparation process

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

# Validation results
VALIDATION_PASSED=true
WARNINGS=()
ERRORS=()

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
    ERRORS+=("$1")
    VALIDATION_PASSED=false
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
    WARNINGS+=("$1")
}

print_info() {
    echo -e "${BLUE}ℹ $1${NC}"
}

# Check if submodules are initialized
validate_submodules_initialized() {
    print_header "Validating Submodules Initialization"
    
    cd "$REPO_ROOT"
    
    local all_initialized=true
    
    git submodule status | while read -r line; do
        commit=$(echo "$line" | awk '{print $1}')
        path=$(echo "$line" | awk '{print $2}')
        
        if [[ $commit == -* ]]; then
            print_error "Submodule $path is not initialized"
            all_initialized=false
        else
            print_success "Submodule $path is initialized"
        fi
    done
    
    if [ "$all_initialized" = true ]; then
        print_success "All submodules are initialized"
    fi
}

# Check if submodules are on tagged versions
validate_submodule_versions() {
    print_header "Validating Submodule Versions"
    
    cd "$REPO_ROOT"
    
    echo ""
    git submodule status | while read -r line; do
        commit=$(echo "$line" | awk '{print $1}' | sed 's/^[+-]//')
        path=$(echo "$line" | awk '{print $2}')
        
        if [ ! -d "$path" ]; then
            print_error "Submodule $path not found"
            continue
        fi
        
        cd "$path"
        
        # Check if on a tagged version
        tag=$(git describe --tags --exact-match 2>/dev/null || echo "")
        branch=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "")
        
        if [ -n "$tag" ]; then
            print_success "$path is on tagged version: $tag"
        elif [ "$branch" != "HEAD" ]; then
            print_warning "$path is on branch $branch (commit: ${commit:0:7}). Consider using a tagged version for release."
        else
            print_warning "$path is on detached HEAD (commit: ${commit:0:7}). Consider using a tagged version for release."
        fi
        
        cd "$REPO_ROOT"
    done
}

# Check for uncommitted changes
validate_no_uncommitted_changes() {
    print_header "Checking for Uncommitted Changes"
    
    cd "$REPO_ROOT"
    
    if git diff-index --quiet HEAD --; then
        print_success "No uncommitted changes in main repository"
    else
        print_error "There are uncommitted changes in main repository"
        echo ""
        echo "Changed files:"
        git status --short
    fi
    
    # Check submodules
    git submodule foreach --quiet --recursive 'if ! git diff-index --quiet HEAD --; then echo "Uncommitted changes in $path"; fi' | while read -r line; do
        if [ -n "$line" ]; then
            print_warning "$line"
        fi
    done
}

# Validate version format
validate_version_format() {
    local version="$1"
    
    if [[ $version =~ ^v?[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
        return 0
    else
        return 1
    fi
}

# Check CHANGELOG.md exists and is updated
validate_changelog() {
    print_header "Validating CHANGELOG.md"
    
    cd "$REPO_ROOT"
    
    if [ ! -f "CHANGELOG.md" ]; then
        print_error "CHANGELOG.md not found"
        return
    fi
    
    print_success "CHANGELOG.md exists"
    
    # Check if CHANGELOG has recent updates
    if git diff HEAD~1 HEAD --name-only | grep -q "CHANGELOG.md"; then
        print_success "CHANGELOG.md has been updated recently"
    else
        print_warning "CHANGELOG.md has not been updated recently"
    fi
}

# Validate version in CMakeLists.txt
validate_cmake_version() {
    local expected_version="$1"
    
    print_header "Validating CMakeLists.txt Version"
    
    cd "$REPO_ROOT"
    
    if [ ! -f "Build/CMake/CMakeLists.txt" ]; then
        print_error "Build/CMake/CMakeLists.txt not found"
        return
    fi
    
    local cmake_version=$(grep "project(ProLifeAll VERSION" Build/CMake/CMakeLists.txt | sed -n 's/.*VERSION \([0-9.]*\).*/\1/p')
    
    if [ -z "$expected_version" ]; then
        print_info "Current CMakeLists.txt version: $cmake_version"
    else
        expected_version="${expected_version#v}"  # Remove 'v' prefix
        if [ "$cmake_version" = "$expected_version" ]; then
            print_success "CMakeLists.txt version matches: $cmake_version"
        else
            print_error "CMakeLists.txt version ($cmake_version) does not match expected version ($expected_version)"
        fi
    fi
}

# Generate submodule version report for release
generate_submodule_report() {
    local output_file="$1"
    
    print_header "Generating Submodule Version Report"
    
    cd "$REPO_ROOT"
    
    {
        echo "# Submodule Versions for Release"
        echo ""
        echo "Generated: $(date '+%Y-%m-%d %H:%M:%S')"
        echo ""
        echo "## Submodules"
        echo ""
        
        git submodule status | while read -r line; do
            commit=$(echo "$line" | awk '{print $1}' | sed 's/^[+-]//')
            path=$(echo "$line" | awk '{print $2}')
            name=$(basename "$path")
            
            if [ -d "$path" ]; then
                cd "$path"
                tag=$(git describe --tags --exact-match 2>/dev/null || echo "N/A")
                branch=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "N/A")
                url=$(git config --get remote.origin.url || echo "N/A")
                cd "$REPO_ROOT"
                
                echo "### $name"
                echo "- **Path:** \`$path\`"
                echo "- **Repository:** $url"
                echo "- **Commit:** \`$commit\`"
                echo "- **Tag:** \`$tag\`"
                echo "- **Branch:** \`$branch\`"
                echo ""
            fi
        done
    } > "$output_file"
    
    print_success "Submodule report saved to $output_file"
}

# Auto-update submodules to latest tagged versions
auto_update_submodules() {
    print_header "Auto-Updating Submodules to Latest Tagged Versions"
    
    cd "$REPO_ROOT"
    
    local updated=false
    
    git submodule status | while read -r line; do
        path=$(echo "$line" | awk '{print $2}')
        
        if [ ! -d "$path" ]; then
            print_warning "Submodule $path not initialized, skipping"
            continue
        fi
        
        cd "$path"
        
        # Fetch latest tags
        git fetch --tags --quiet
        
        # Get latest tag
        latest_tag=$(git tag -l --sort=-v:refname | grep -E "^v?[0-9]+\.[0-9]+\.[0-9]+$" | head -1)
        
        if [ -z "$latest_tag" ]; then
            print_warning "No semantic version tags found for $path, skipping"
            cd "$REPO_ROOT"
            continue
        fi
        
        current_tag=$(git describe --tags --exact-match 2>/dev/null || echo "")
        
        if [ "$current_tag" = "$latest_tag" ]; then
            print_info "$path is already on latest tag: $latest_tag"
        else
            print_info "$path: Updating from $current_tag to $latest_tag"
            git checkout "$latest_tag" --quiet
            updated=true
            cd "$REPO_ROOT"
            git add "$path"
            print_success "$path updated to $latest_tag"
        fi
        
        cd "$REPO_ROOT"
    done
    
    if [ "$updated" = true ]; then
        print_success "Submodules updated. Please review changes with 'git status'"
    else
        print_info "All submodules are already on latest tagged versions"
    fi
}

# Pin submodules with validation
pin_submodules_with_validation() {
    print_header "Pinning Submodules with Validation"
    
    cd "$REPO_ROOT"
    
    # First validate
    validate_submodule_versions
    
    echo ""
    read -p "Proceed with pinning submodules? (y/n) " -n 1 -r
    echo
    
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        git submodule foreach 'git rev-parse HEAD > /dev/null'
        git add -A
        print_success "Submodules pinned"
    else
        print_info "Pinning cancelled"
    fi
}

# Generate release summary
generate_release_summary() {
    local version="$1"
    
    print_header "Release Summary"
    
    echo ""
    echo "Version: $version"
    echo "Date: $(date '+%Y-%m-%d')"
    echo ""
    echo "Submodules:"
    
    cd "$REPO_ROOT"
    git submodule status | while read -r line; do
        commit=$(echo "$line" | awk '{print $1}' | sed 's/^[+-]//')
        path=$(echo "$line" | awk '{print $2}')
        name=$(basename "$path")
        
        if [ -d "$path" ]; then
            cd "$path"
            tag=$(git describe --tags --exact-match 2>/dev/null || echo "commit ${commit:0:7}")
            echo "  - $name: $tag"
            cd "$REPO_ROOT"
        fi
    done
    
    echo ""
    
    if [ ${#ERRORS[@]} -eq 0 ]; then
        print_success "No errors found"
    else
        echo "Errors:"
        for error in "${ERRORS[@]}"; do
            echo "  - $error"
        done
    fi
    
    if [ ${#WARNINGS[@]} -gt 0 ]; then
        echo ""
        echo "Warnings:"
        for warning in "${WARNINGS[@]}"; do
            echo "  - $warning"
        done
    fi
}

# Full validation workflow
full_validation() {
    local version="$1"
    
    print_header "ProLife Release Validation"
    echo ""
    
    validate_submodules_initialized
    echo ""
    
    validate_no_uncommitted_changes
    echo ""
    
    validate_submodule_versions
    echo ""
    
    validate_changelog
    echo ""
    
    validate_cmake_version "$version"
    echo ""
    
    generate_release_summary "$version"
    
    if [ "$VALIDATION_PASSED" = true ] && [ ${#WARNINGS[@]} -eq 0 ]; then
        echo ""
        print_success "✅ All validations passed! Ready for release."
        return 0
    elif [ "$VALIDATION_PASSED" = true ]; then
        echo ""
        print_warning "⚠️  Validation passed with warnings. Review warnings before proceeding."
        return 0
    else
        echo ""
        print_error "❌ Validation failed. Please fix errors before proceeding with release."
        return 1
    fi
}

# Show help
show_help() {
    cat << EOF
ProLife Release Validation and Automation Script

Usage: $0 [COMMAND] [OPTIONS]

Commands:
  validate [VERSION]           Run full validation for release
  auto-update                  Auto-update all submodules to latest tagged versions
  pin                          Pin submodules with validation
  report [OUTPUT_FILE]         Generate submodule version report
  summary [VERSION]            Generate release summary
  help                         Show this help message

Examples:
  $0 validate v2.1.0          # Validate release preparation for version 2.1.0
  $0 auto-update              # Update all submodules to latest tags
  $0 pin                      # Pin submodules after validation
  $0 report versions.md       # Generate version report
  $0 summary v2.1.0           # Generate release summary

Typical release workflow:
  1. $0 validate              # Check current state
  2. $0 auto-update           # Update submodules to latest versions
  3. # Test the integration
  4. $0 validate v2.1.0       # Validate for specific version
  5. $0 report                # Generate version report
  6. $0 pin                   # Pin submodules
  7. # Commit and push

EOF
}

# Main script
main() {
    if [ $# -eq 0 ]; then
        show_help
        exit 0
    fi
    
    case "$1" in
        validate)
            full_validation "$2"
            ;;
        auto-update)
            auto_update_submodules
            ;;
        pin)
            pin_submodules_with_validation
            ;;
        report)
            output_file="${2:-SUBMODULE_VERSIONS.md}"
            generate_submodule_report "$output_file"
            ;;
        summary)
            generate_release_summary "$2"
            ;;
        help|--help|-h)
            show_help
            ;;
        *)
            print_error "Unknown command: $1"
            echo ""
            show_help
            exit 1
            ;;
    esac
}

# Run main function
main "$@"
