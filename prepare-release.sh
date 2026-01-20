#!/bin/bash
# ProLife Release Preparation Script
# This script helps manage releases and submodule versions

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

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

# Check if we're in a git repository
check_git_repo() {
    if ! git rev-parse --is-inside-work-tree > /dev/null 2>&1; then
        print_error "Not in a git repository"
        exit 1
    fi
}

# Show current submodule status
show_submodule_status() {
    print_header "Current Submodule Status"
    
    cd "$SCRIPT_DIR"
    
    echo ""
    git submodule status | while read -r line; do
        commit=$(echo "$line" | awk '{print $1}')
        path=$(echo "$line" | awk '{print $2}')
        ref=$(echo "$line" | awk '{print $3}' | tr -d '()')
        
        if [[ $commit == -* ]]; then
            commit=${commit:1}
            print_warning "$path: Not initialized"
        elif [[ $commit == +* ]]; then
            commit=${commit:1}
            print_warning "$path: Modified (commit: $commit)"
        else
            print_info "$path: $commit"
        fi
        
        # Get current branch/tag if available
        if [ -d "$path" ]; then
            cd "$path"
            current_tag=$(git describe --tags --exact-match 2>/dev/null || echo "")
            current_branch=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "")
            
            if [ -n "$current_tag" ]; then
                echo "  └─ Tag: $current_tag"
            elif [ "$current_branch" != "HEAD" ]; then
                echo "  └─ Branch: $current_branch"
            else
                echo "  └─ Detached HEAD"
            fi
            cd "$SCRIPT_DIR"
        fi
        echo ""
    done
}

# List available tags for a submodule
list_submodule_tags() {
    local submodule_path="$1"
    
    if [ ! -d "$submodule_path" ]; then
        print_error "Submodule $submodule_path not found or not initialized"
        return 1
    fi
    
    cd "$submodule_path"
    print_info "Available tags for $(basename "$submodule_path"):"
    git tag -l | tail -20
    cd "$SCRIPT_DIR"
}

# Update submodule to specific version
update_submodule_version() {
    local submodule_path="$1"
    local version="$2"
    
    if [ ! -d "$submodule_path" ]; then
        print_error "Submodule $submodule_path not found"
        return 1
    fi
    
    print_info "Updating $submodule_path to $version..."
    
    cd "$submodule_path"
    
    # Fetch latest
    git fetch --tags
    
    # Checkout the version
    if git rev-parse "$version" >/dev/null 2>&1; then
        git checkout "$version"
        print_success "Updated $submodule_path to $version"
    else
        print_error "Version $version not found in $submodule_path"
        cd "$SCRIPT_DIR"
        return 1
    fi
    
    cd "$SCRIPT_DIR"
    
    # Stage the submodule change
    git add "$submodule_path"
}

# Pin all submodules to their current commits
pin_submodules() {
    print_header "Pinning Submodules to Current Commits"
    
    cd "$SCRIPT_DIR"
    
    git submodule foreach 'git rev-parse HEAD > /dev/null'
    git add -A
    
    print_success "All submodules pinned to their current commits"
    print_info "Run 'git status' to see changes"
}

# Update all submodules to latest on their tracking branch
update_all_submodules() {
    print_header "Updating All Submodules"
    
    cd "$SCRIPT_DIR"
    
    print_info "Fetching updates for all submodules..."
    git submodule update --remote --recursive
    
    print_success "All submodules updated"
    show_submodule_status
}

# Initialize all submodules
init_submodules() {
    print_header "Initializing Submodules"
    
    cd "$SCRIPT_DIR"
    
    print_info "Initializing and updating all submodules..."
    git submodule update --init --recursive
    
    print_success "All submodules initialized"
}

# Create a release branch
create_release_branch() {
    local version="$1"
    
    if [ -z "$version" ]; then
        print_error "Version is required (e.g., 2.1.0)"
        return 1
    fi
    
    # Validate version format (MAJOR.MINOR.PATCH)
    if ! [[ $version =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
        print_error "Invalid version format. Use MAJOR.MINOR.PATCH (e.g., 2.1.0)"
        return 1
    fi
    
    local branch_name="release/v$version"
    
    print_header "Creating Release Branch"
    
    cd "$SCRIPT_DIR"
    
    # Check if branch already exists
    if git rev-parse --verify "$branch_name" >/dev/null 2>&1; then
        print_error "Branch $branch_name already exists"
        return 1
    fi
    
    # Check for uncommitted changes
    if ! git diff-index --quiet HEAD --; then
        print_warning "You have uncommitted changes. Please commit or stash them first."
        return 1
    fi
    
    # Create the branch
    print_info "Creating branch $branch_name from current branch..."
    git checkout -b "$branch_name"
    
    print_success "Created release branch: $branch_name"
    print_info "Next steps:"
    echo "  1. Update version numbers in your code"
    echo "  2. Update CHANGELOG.md"
    echo "  3. Pin submodules with: $0 --pin-submodules"
    echo "  4. Commit changes: git commit -m 'Prepare release v$version'"
    echo "  5. Push branch: git push -u origin $branch_name"
}

# Generate submodule version report
generate_version_report() {
    print_header "Submodule Version Report"
    
    cd "$SCRIPT_DIR"
    
    local report_file="SUBMODULE_VERSIONS.md"
    
    {
        echo "# ProLife Submodule Versions"
        echo ""
        echo "Generated: $(date)"
        echo ""
        echo "## Current Versions"
        echo ""
        
        git submodule status | while read -r line; do
            commit=$(echo "$line" | awk '{print $1}' | sed 's/^[+-]//')
            path=$(echo "$line" | awk '{print $2}')
            name=$(basename "$path")
            
            if [ -d "$path" ]; then
                cd "$path"
                tag=$(git describe --tags --exact-match 2>/dev/null || echo "N/A")
                branch=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "N/A")
                cd "$SCRIPT_DIR"
                
                echo "### $name"
                echo "- Path: \`$path\`"
                echo "- Commit: \`$commit\`"
                echo "- Tag: \`$tag\`"
                echo "- Branch: \`$branch\`"
                echo ""
            fi
        done
    } > "$report_file"
    
    print_success "Version report generated: $report_file"
    cat "$report_file"
}

# Interactive mode to update submodule versions
interactive_update() {
    print_header "Interactive Submodule Version Update"
    
    cd "$SCRIPT_DIR"
    
    # Get list of submodules
    local submodules=($(git submodule status | awk '{print $2}'))
    
    if [ ${#submodules[@]} -eq 0 ]; then
        print_warning "No submodules found"
        return 0
    fi
    
    for submodule in "${submodules[@]}"; do
        local name=$(basename "$submodule")
        echo ""
        print_info "Submodule: $name ($submodule)"
        
        if [ ! -d "$submodule" ]; then
            print_warning "Not initialized. Skipping."
            continue
        fi
        
        cd "$submodule"
        local current_commit=$(git rev-parse --short HEAD)
        local current_tag=$(git describe --tags --exact-match 2>/dev/null || echo "none")
        echo "  Current: $current_commit (tag: $current_tag)"
        
        # Show recent tags
        echo "  Recent tags:"
        git tag -l | tail -5 | sed 's/^/    /'
        
        cd "$SCRIPT_DIR"
        
        read -p "  Update to version (or press Enter to skip): " version
        
        if [ -n "$version" ]; then
            update_submodule_version "$submodule" "$version"
        else
            print_info "Skipped $name"
        fi
    done
    
    echo ""
    print_success "Interactive update complete"
    print_info "Review changes with: git status"
}

# Show help
show_help() {
    cat << EOF
ProLife Release Preparation Script

Usage: $0 [OPTION]

Options:
  --status, -s                    Show current submodule status
  --init                          Initialize all submodules
  --update-all                    Update all submodules to latest
  --pin-submodules                Pin all submodules to current commits
  --create-release VERSION        Create a release branch (e.g., 2.1.0)
  --update-submodule PATH VERSION Update specific submodule to version
  --list-tags PATH                List available tags for a submodule
  --interactive, -i               Interactive submodule version update
  --report, -r                    Generate submodule version report
  --help, -h                      Show this help message

Examples:
  $0 --status
  $0 --init
  $0 --create-release 2.1.0
  $0 --update-submodule 3rdParty/Acf v1.2.3
  $0 --list-tags 3rdParty/Acf
  $0 --interactive
  $0 --pin-submodules
  $0 --report

Workflow for creating a release:
  1. $0 --status                          # Check current state
  2. $0 --create-release 2.1.0            # Create release branch
  3. $0 --interactive                     # Update submodule versions
  4. # Update version numbers in code and CHANGELOG.md
  5. $0 --pin-submodules                  # Pin to current commits
  6. git commit -m "Prepare release v2.1.0"
  7. git push -u origin release/v2.1.0

EOF
}

# Main script
main() {
    check_git_repo
    
    if [ $# -eq 0 ]; then
        show_help
        exit 0
    fi
    
    case "$1" in
        --status|-s)
            show_submodule_status
            ;;
        --init)
            init_submodules
            ;;
        --update-all)
            update_all_submodules
            ;;
        --pin-submodules)
            pin_submodules
            ;;
        --create-release)
            if [ -z "$2" ]; then
                print_error "Version is required"
                echo "Usage: $0 --create-release VERSION"
                exit 1
            fi
            create_release_branch "$2"
            ;;
        --update-submodule)
            if [ -z "$2" ] || [ -z "$3" ]; then
                print_error "Submodule path and version are required"
                echo "Usage: $0 --update-submodule PATH VERSION"
                exit 1
            fi
            update_submodule_version "$2" "$3"
            ;;
        --list-tags)
            if [ -z "$2" ]; then
                print_error "Submodule path is required"
                echo "Usage: $0 --list-tags PATH"
                exit 1
            fi
            list_submodule_tags "$2"
            ;;
        --interactive|-i)
            interactive_update
            ;;
        --report|-r)
            generate_version_report
            ;;
        --help|-h)
            show_help
            ;;
        *)
            print_error "Unknown option: $1"
            echo ""
            show_help
            exit 1
            ;;
    esac
}

# Run main function
main "$@"
