# ProLife Release Process Documentation

This document provides a comprehensive guide to the ProLife release process, including detailed instructions for managing submodule versions, preparing releases, and working with both Linux/macOS (bash scripts) and Windows (batch scripts).

## Table of Contents

- [Overview](#overview)
- [Release Strategy](#release-strategy)
- [Branch Structure](#branch-structure)
- [Preparing a Release](#preparing-a-release)
- [Managing Submodule Versions](#managing-submodule-versions)
- [Release Branch Workflow](#release-branch-workflow)
- [Rollback Procedures](#rollback-procedures)
- [Cross-Platform Tools](#cross-platform-tools)
- [Git Commands Reference](#git-commands-reference)
- [Troubleshooting](#troubleshooting)

## Overview

ProLife follows semantic versioning (MAJOR.MINOR.PATCH) and uses Git submodules for managing dependencies. The release process is automated through scripts available for both Unix-like systems (Linux/macOS) and Windows.

### Key Principles

1. **Semantic Versioning**: All releases follow `MAJOR.MINOR.PATCH` format
2. **Tagged Submodules**: Production releases always use tagged versions of submodules
3. **Release Branches**: Each release is prepared in a dedicated `release/vX.Y.Z` branch
4. **Automated Validation**: Scripts validate release readiness before deployment
5. **Cross-Platform Support**: All release tools work on Windows, Linux, and macOS

## Release Strategy

### Version Types

- **Major Release** (X.0.0): Breaking changes, major new features
- **Minor Release** (X.Y.0): New features, backward compatible
- **Patch Release** (X.Y.Z): Bug fixes, no new features
- **Release Candidates** (X.Y.Z-rc1): Pre-release testing versions
- **Hotfixes**: Critical production fixes from main branch

### Release Cadence

- **Minor Releases**: Every 2-3 months
- **Patch Releases**: As needed for bug fixes
- **Hotfixes**: Immediately for critical issues

## Branch Structure

ProLife uses a structured branching model:

```
main (production)
  ├── release/v2.1.0 (release preparation)
  ├── hotfix/v2.0.1 (critical fixes)
  └── develop (integration)
      ├── feature/new-feature (new features)
      └── bugfix/fix-issue (bug fixes)
```

### Branch Descriptions

#### main Branch
- **Purpose**: Production-ready code
- **Protection**: Direct commits not allowed
- **Updates**: Only from merged release branches or hotfixes
- **Tags**: All release tags (v2.0.0, v2.1.0, etc.) point to commits in main

#### release/vX.Y.Z Branches
- **Purpose**: Prepare specific release versions
- **Created From**: main or develop
- **Lifetime**: Deleted after merge to main
- **Activities**: 
  - Update submodule versions
  - Update CHANGELOG.md
  - Pin dependencies
  - Final testing and validation

#### develop Branch (if used)
- **Purpose**: Integration branch for features
- **Updates**: Regular merges from feature branches
- **Stability**: Should always build and pass tests

#### feature/* Branches
- **Purpose**: Develop new features
- **Created From**: develop
- **Merged To**: develop
- **Naming**: feature/descriptive-name

#### hotfix/* Branches
- **Purpose**: Critical production fixes
- **Created From**: main (from the tag needing fix)
- **Merged To**: Both main and develop
- **Naming**: hotfix/vX.Y.Z

### Why Transfer Submodule States from Release to Main?

When you prepare a release branch, you:

1. **Select specific submodule versions** that have been tested together
2. **Pin these versions** to ensure reproducibility
3. **Validate the integration** works correctly

When you merge the release branch back to main:

1. **Main gets the exact tested configuration**
2. **Submodule commits are recorded** in the main branch
3. **Release tags point to this exact state**
4. **Future builds from this tag** will use the same submodule versions

This ensures that:
- ✅ Releases are reproducible
- ✅ Each release has a known, tested configuration
- ✅ You can checkout any release tag and get working code
- ✅ Submodule versions are part of the release definition

**Example:**
```bash
# Release branch has:
# - 3rdParty/Acf @ v2.0.0 (commit abc123)
# - 3rdParty/ImtCore @ v1.5.0 (commit def456)

# After merging to main:
git checkout v2.1.0
git submodule update --init --recursive
# This will checkout exactly:
# - Acf at v2.0.0 (abc123)
# - ImtCore at v1.5.0 (def456)
# Same tested configuration!
```

## Preparing a Release

### Quick Start (Automated)

**Linux/macOS:**
```bash
# One-command release preparation
./tools/quick-release.sh 2.1.0

# Follow the prompts and instructions
```

**Windows:**
```cmd
REM One-command release preparation
tools\quick-release.bat 2.1.0

REM Follow the prompts and instructions
```

### Step-by-Step Manual Process

#### Step 1: Validate Current State

Check your repository status before starting:

**Linux/macOS:**
```bash
./tools/validate-release.sh validate
```

**Windows:**
```cmd
tools\validate-release.bat validate
```

This checks:
- ✅ All submodules are initialized
- ✅ No uncommitted changes
- ✅ Submodule versions are appropriate
- ✅ CHANGELOG.md exists

#### Step 2: Create Release Branch

**Using Scripts:**

Linux/macOS:
```bash
./prepare-release.sh --create-release 2.1.0
```

Windows:
```cmd
prepare-release.bat --create-release 2.1.0
```

**Manual Git Commands:**
```bash
# Check current branch and status
git status
git checkout main
git pull origin main

# Create release branch
git checkout -b release/v2.1.0

# Verify
git branch
```

#### Step 3: Initialize Submodules

If submodules are not initialized:

**Linux/macOS:**
```bash
./prepare-release.sh --init
```

**Windows:**
```cmd
prepare-release.bat --init
```

**Manual:**
```bash
git submodule update --init --recursive
```

#### Step 4: Update Submodules to Target Versions

See [Managing Submodule Versions](#managing-submodule-versions) section below.

#### Step 5: Test Integration

**Build the project:**

Linux/macOS:
```bash
mkdir -p build-release-test
cd build-release-test
cmake ../Build/CMake -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cd ..
```

Windows:
```cmd
mkdir build-release-test
cd build-release-test
cmake ..\Build\CMake -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cd ..
```

**Run tests:**
```bash
cd build-release-test
ctest --config Release --output-on-failure
cd ..
```

#### Step 6: Update CHANGELOG.md

Edit CHANGELOG.md manually to document the release:

```markdown
## [2.1.0] - 2026-01-20

### Added
- New feature X
- Support for Y format

### Changed
- Improved performance of Z

### Fixed
- Fixed bug in component A
- Fixed memory leak in B

### Dependencies
- Updated Acf to v2.0.0
- Updated ImtCore to v1.5.0
```

#### Step 7: Update Version in CMakeLists.txt

Edit `Build/CMake/CMakeLists.txt`:

```cmake
project(ProLifeAll VERSION 2.1.0)
```

#### Step 8: Generate Submodule Version Report

**Linux/macOS:**
```bash
./tools/validate-release.sh report RELEASE_SUBMODULES_v2.1.0.md
```

**Windows:**
```cmd
tools\validate-release.bat report RELEASE_SUBMODULES_v2.1.0.md
```

This creates a detailed report of all submodule versions for documentation.

#### Step 9: Pin Submodules

**Linux/macOS:**
```bash
./prepare-release.sh --pin-submodules
```

**Windows:**
```cmd
prepare-release.bat --pin-submodules
```

**Manual:**
```bash
git submodule foreach 'git rev-parse HEAD > /dev/null'
git add -A
```

This ensures all submodule references are committed at their current commits.

#### Step 10: Final Validation

**Linux/macOS:**
```bash
./tools/validate-release.sh validate v2.1.0
```

**Windows:**
```cmd
tools\validate-release.bat validate v2.1.0
```

Address any errors or warnings before proceeding.

#### Step 11: Commit Release Changes

```bash
git status  # Review changes
git add .
git commit -m "chore: prepare release v2.1.0

- Updated submodules to stable versions
- Updated CHANGELOG.md with release notes
- Updated version to 2.1.0 in CMakeLists.txt
- Pinned all dependencies

Dependencies:
- Acf v2.0.0
- ImtCore v1.5.0
"
```

#### Step 12: Push Release Branch

```bash
git push -u origin release/v2.1.0
```

#### Step 13: Create Pull Request

Create a PR from `release/v2.1.0` to `main` for review.

#### Step 14: After PR Approval

Once the PR is approved and merged to main:

```bash
# Switch to main
git checkout main
git pull origin main

# Create and push tag
git tag -a v2.1.0 -m "Release version 2.1.0"
git push origin v2.1.0

# Create GitHub Release (manual or via GitHub Actions)
```

## Managing Submodule Versions

### Understanding Submodule Versioning

ProLife depends on several repositories managed as Git submodules:
- `3rdParty/Acf` - Advanced Computing Framework
- `3rdParty/ImtCore` - Imaging Tools Core library
- `3rdParty/Lisa`, `3rdParty/Puma`, `3rdParty/Agentino` - Application components

Each submodule can be at:
- **A specific tag** (e.g., `v2.0.0`) - ✅ **Recommended for releases**
- **A branch** (e.g., `main`) - ⚠️ Use for development only
- **A specific commit** (e.g., `abc123def`) - ⚠️ Hard to track

### Automated Submodule Updates

#### Auto-Update to Latest Tagged Versions

The scripts can automatically find and update all submodules to their latest semantic version tags:

**Linux/macOS:**
```bash
./tools/validate-release.sh auto-update
```

**Windows:**
```cmd
tools\validate-release.bat auto-update
```

This will:
1. Fetch all tags from each submodule
2. Find the latest semantic version tag (vX.Y.Z format)
3. Update each submodule to that version
4. Stage changes for commit

#### Set Specific Submodule Versions

Use the version management script:

**Linux/macOS:**
```bash
./tools/set-submodule-versions.sh
```

**Windows:**
```cmd
tools\set-submodule-versions.bat
```

This interactive script will:
1. List all submodules
2. Show available versions for each
3. Allow you to select desired versions
4. Update submodules accordingly

### Manual Submodule Updates

#### Update One Submodule

**Linux/macOS:**
```bash
# List available tags
./prepare-release.sh --list-tags 3rdParty/Acf

# Update to specific version
./prepare-release.sh --update-submodule 3rdParty/Acf v2.0.0
```

**Windows:**
```cmd
REM List available tags
prepare-release.bat --list-tags 3rdParty\Acf

REM Update to specific version
prepare-release.bat --update-submodule 3rdParty\Acf v2.0.0
```

**Manual Git Commands:**
```bash
# Navigate to submodule
cd 3rdParty/Acf

# Fetch latest changes
git fetch --tags

# List available tags
git tag -l

# Checkout desired version
git checkout v2.0.0

# Return to main repo
cd ../..

# Stage the change
git add 3rdParty/Acf

# Verify
git status
```

#### Update All Submodules

**Linux/macOS:**
```bash
./prepare-release.sh --update-all
```

**Windows:**
```cmd
prepare-release.bat --update-all
```

**Manual:**
```bash
git submodule update --remote --recursive
```

⚠️ **Warning**: This updates submodules to the latest commit on their tracking branch, not necessarily to tagged versions.

### Checking Current Submodule Versions

**Linux/macOS:**
```bash
./prepare-release.sh --status
```

**Windows:**
```cmd
prepare-release.bat --status
```

**Manual:**
```bash
# Show submodule status
git submodule status

# Show detailed info for each submodule
git submodule foreach 'echo "=== $name ===" && git describe --tags 2>/dev/null || echo "No tag" && git rev-parse --abbrev-ref HEAD'
```

## Release Branch Workflow

### Creating a Release Branch

A release branch is where you:
1. ✅ Finalize the version
2. ✅ Pin submodule versions
3. ✅ Update documentation
4. ✅ Run final tests
5. ✅ Prepare for production

```bash
# From main or develop
git checkout main
git pull origin main

# Create release branch
git checkout -b release/v2.1.0

# Now work in this branch
```

### Working in Release Branch

While in a release branch:

```bash
# Update submodules
./tools/validate-release.sh auto-update  # or .bat on Windows

# Test
mkdir build && cd build
cmake ../Build/CMake
cmake --build . --config Release
ctest --config Release
cd ..

# Update documentation
vim CHANGELOG.md

# Pin submodules
./prepare-release.sh --pin-submodules  # or .bat on Windows

# Commit
git add .
git commit -m "chore: prepare release v2.1.0"

# Push
git push -u origin release/v2.1.0
```

### Merging Release Branch to Main

After PR approval:

```bash
# Merge to main
git checkout main
git merge --no-ff release/v2.1.0
git push origin main

# Tag the release
git tag -a v2.1.0 -m "Release version 2.1.0"
git push origin v2.1.0

# If you have a develop branch, merge there too
git checkout develop
git merge --no-ff release/v2.1.0
git push origin develop

# Delete release branch (optional)
git branch -d release/v2.1.0
git push origin --delete release/v2.1.0
```

### Why Merge to Main?

When you merge a release branch to main:

1. **Submodule States Transfer**: The exact commit hashes of submodules are recorded
2. **Tags Point to Complete State**: The v2.1.0 tag includes both code and submodule references
3. **Reproducibility**: Anyone checking out v2.1.0 gets the exact tested configuration
4. **History Preservation**: The merge commit shows what went into the release

```bash
# After merging and tagging
git checkout v2.1.0
git submodule update --init --recursive
# ← This gives you the EXACT state that was tested and released
```

## Rollback Procedures

### Rolling Back to Previous Release

If a release has issues, you can rollback:

#### Option 1: Quick Rollback (Deploy Previous Version)

```bash
# Checkout previous release tag
git checkout v2.0.0

# Update submodules to match
git submodule update --init --recursive

# Build
mkdir build && cd build
cmake ../Build/CMake -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

# Deploy this build
```

#### Option 2: Create Hotfix Release

```bash
# Start from previous release
git checkout v2.0.0
git checkout -b hotfix/v2.0.1

# Make necessary fixes
# ... edit files ...

# Test
mkdir build && cd build
cmake ../Build/CMake
cmake --build . --config Release
ctest --config Release
cd ..

# Update version to 2.0.1
vim Build/CMake/CMakeLists.txt

# Commit
git add .
git commit -m "fix: critical issue in production

- Fixed bug X
- Reverted problematic change Y
"

# Merge to main
git checkout main
git merge --no-ff hotfix/v2.0.1

# Tag
git tag -a v2.0.1 -m "Hotfix release 2.0.1"
git push origin main --tags

# Merge to develop too
git checkout develop
git merge --no-ff hotfix/v2.0.1
git push origin develop
```

### Rolling Back a Submodule

If a specific submodule version causes issues:

**Linux/macOS:**
```bash
# List available versions
./prepare-release.sh --list-tags 3rdParty/Acf

# Rollback to previous version
./prepare-release.sh --update-submodule 3rdParty/Acf v1.9.0

# Test the change
mkdir build && cd build
cmake ../Build/CMake
cmake --build . --config Release
ctest --config Release
cd ..

# Commit
git add 3rdParty/Acf
git commit -m "chore(deps): rollback Acf to v1.9.0

Regression found in v2.0.0 - issue #123
"
```

**Windows:**
```cmd
REM List available versions
prepare-release.bat --list-tags 3rdParty\Acf

REM Rollback to previous version
prepare-release.bat --update-submodule 3rdParty\Acf v1.9.0

REM Test and commit as above
```

### Building an Old Release

To build any previous release:

```bash
# Checkout the release tag
git checkout v2.0.0

# Update submodules to match that release
git submodule update --init --recursive

# Verify versions
git submodule status

# Build
mkdir build-v2.0.0
cd build-v2.0.0
cmake ../Build/CMake -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

# Run
./Bin/ProLifeClient  # or appropriate executable
```

This works because:
- The tag v2.0.0 points to a commit in main
- That commit includes submodule references at specific commits
- `git submodule update` checks out those exact commits
- You get the exact code that was released

## Cross-Platform Tools

ProLife provides equivalent tools for both Unix-like systems and Windows.

### Available Scripts

| Purpose | Linux/macOS | Windows | Location |
|---------|-------------|---------|----------|
| Quick release automation | `quick-release.sh` | `quick-release.bat` | `tools/` |
| Release validation | `validate-release.sh` | `validate-release.bat` | `tools/` |
| Submodule version management | `set-submodule-versions.sh` | `set-submodule-versions.bat` | `tools/` |
| Manual release prep | `prepare-release.sh` | `prepare-release.bat` | Root |
| Environment setup | `setup-environment.sh` | `setup-environment.bat` | Root |

### Using Scripts on Linux/macOS

```bash
# Make scripts executable (first time only)
chmod +x tools/quick-release.sh
chmod +x tools/validate-release.sh
chmod +x prepare-release.sh

# Run scripts
./tools/quick-release.sh 2.1.0
./tools/validate-release.sh validate
./prepare-release.sh --status
```

### Using Scripts on Windows

```cmd
REM No need to set permissions, just run directly
tools\quick-release.bat 2.1.0
tools\validate-release.bat validate
prepare-release.bat --status
```

### Using Scripts on Windows with Git Bash

If you have Git for Windows installed with Git Bash:

```bash
# You can use .sh scripts in Git Bash
./tools/quick-release.sh 2.1.0

# Or use .bat scripts
./tools/quick-release.bat 2.1.0
```

**Recommendation**: On Windows, prefer the `.bat` scripts in Command Prompt or PowerShell for best compatibility. Use `.sh` scripts only if you're in Git Bash.

### Cross-Platform Considerations

#### Path Separators

- **Linux/macOS**: Use forward slashes `/`
  ```bash
  ./tools/quick-release.sh
  cd 3rdParty/Acf
  ```

- **Windows**: Use backslashes `\` in Command Prompt
  ```cmd
  tools\quick-release.bat
  cd 3rdParty\Acf
  ```

#### Script Extensions

- **Linux/macOS**: `.sh` extension (Bash scripts)
- **Windows**: `.bat` extension (Batch scripts)

#### Line Endings

- Git is configured to handle line endings automatically
- `.sh` files use LF (Unix line endings)
- `.bat` files use CRLF (Windows line endings)

## Git Commands Reference

### Common Release Commands

#### Repository Status

```bash
# Check current status
git status

# Check current branch
git branch

# View recent commits
git log --oneline -10

# Show submodule status
git submodule status
```

#### Branch Management

```bash
# List all branches
git branch -a

# Create new branch
git checkout -b release/v2.1.0

# Switch branch
git checkout main

# Delete branch
git branch -d release/v2.1.0

# Push branch
git push -u origin release/v2.1.0
```

#### Submodule Commands

```bash
# Initialize submodules
git submodule update --init --recursive

# Update submodules to latest on tracking branch
git submodule update --remote

# Check submodule status
git submodule status

# Execute command in all submodules
git submodule foreach 'git fetch --tags'

# Show submodule details
git submodule foreach 'echo "=== $name ===" && git describe --tags'
```

#### Tagging

```bash
# Create annotated tag
git tag -a v2.1.0 -m "Release version 2.1.0"

# List tags
git tag -l

# Show tag details
git show v2.1.0

# Push tag to remote
git push origin v2.1.0

# Push all tags
git push origin --tags

# Delete tag locally
git tag -d v2.1.0

# Delete tag remotely
git push origin --delete v2.1.0
```

#### Merge and Revert

```bash
# Merge with merge commit
git merge --no-ff release/v2.1.0

# Abort merge
git merge --abort

# View merge conflicts
git diff --name-only --diff-filter=U

# Reset to previous commit
git reset --hard HEAD~1

# Reset to specific commit
git reset --hard abc123
```

#### Release Workflow Commands

```bash
# Complete release workflow
git checkout main
git pull origin main
git checkout -b release/v2.1.0

# ... prepare release ...

git add .
git commit -m "chore: prepare release v2.1.0"
git push -u origin release/v2.1.0

# After PR merge
git checkout main
git pull origin main
git tag -a v2.1.0 -m "Release version 2.1.0"
git push origin v2.1.0
```

## Troubleshooting

### Common Issues

#### 1. Submodule Not Initialized

**Symptom:**
```
fatal: not a git repository (or any of the parent directories): .git
```

**Solution:**

Linux/macOS:
```bash
./prepare-release.sh --init
```

Windows:
```cmd
prepare-release.bat --init
```

Or manually:
```bash
git submodule update --init --recursive
```

#### 2. Detached HEAD in Submodule

**Symptom:**
```
HEAD detached at v2.0.0
```

**Explanation:** This is normal for pinned submodules. For development, you might want to be on a branch.

**Solution (if needed):**
```bash
cd 3rdParty/Acf
git checkout main  # or another branch
cd ../..
git add 3rdParty/Acf
```

#### 3. Uncommitted Changes

**Symptom:**
```
error: You have uncommitted changes
```

**Solution:**
```bash
# Review changes
git status

# Option 1: Commit them
git add .
git commit -m "Description of changes"

# Option 2: Stash them
git stash save "Work in progress"

# Option 3: Discard them (careful!)
git checkout .
```

#### 4. Merge Conflicts in Submodules

**Symptom:**
```
CONFLICT (submodule)
```

**Solution:**
```bash
# Check conflicted submodules
git status

# For each conflicted submodule:
cd 3rdParty/Acf
git fetch --all
git log --oneline --graph --all -10

# Choose which commit to use
git checkout abc123  # or v2.0.0

cd ../..
git add 3rdParty/Acf

# After resolving all conflicts
git commit
```

#### 5. Script Permission Denied (Linux/macOS)

**Symptom:**
```
bash: ./tools/quick-release.sh: Permission denied
```

**Solution:**
```bash
chmod +x tools/quick-release.sh
chmod +x tools/validate-release.sh
chmod +x prepare-release.sh
```

#### 6. Build Fails After Submodule Update

**Symptom:**
```
error: incompatible API version
```

**Solution:**

This indicates breaking changes in a submodule. Options:

```bash
# Option 1: Rollback to previous version
./prepare-release.sh --list-tags 3rdParty/Acf
./prepare-release.sh --update-submodule 3rdParty/Acf v1.9.0

# Option 2: Fix compatibility issues in code
# ... make necessary code changes ...

# Option 3: Use a different compatible version
./prepare-release.sh --update-submodule 3rdParty/Acf v2.0.1
```

Always test after updating submodules!

#### 7. Tag Already Exists

**Symptom:**
```
fatal: tag 'v2.1.0' already exists
```

**Solution:**
```bash
# Check existing tag
git show v2.1.0

# Delete and recreate if needed
git tag -d v2.1.0
git push origin --delete v2.1.0

# Create new tag
git tag -a v2.1.0 -m "Release version 2.1.0"
git push origin v2.1.0
```

#### 8. Cannot Push to Protected Branch

**Symptom:**
```
error: GH006: Protected branch update failed
```

**Solution:**

You can't push directly to main if it's protected. Use the proper workflow:

```bash
# Create release branch
git checkout -b release/v2.1.0

# Make changes and push
git push -u origin release/v2.1.0

# Create Pull Request on GitHub
# After approval, PR can be merged to main
```

### Getting Help

1. **Check this documentation** - Most common scenarios are covered
2. **Run validation scripts** - They often identify issues automatically
3. **Check script help**:
   ```bash
   ./tools/quick-release.sh --help
   ./tools/validate-release.sh help
   ```
4. **Review related documentation**:
   - [RELEASE_GUIDE_RU.md](RELEASE_GUIDE_RU.md) - Russian release guide
   - [RELEASE_GUIDE.md](RELEASE_GUIDE.md) - English release guide
   - [RELEASE_STRATEGY.md](RELEASE_STRATEGY.md) - Release strategy
   - [tools/README.md](tools/README.md) - Tools documentation
5. **Contact development team** - For complex issues

### Platform-Specific Issues

#### Windows-Specific

**Issue**: Scripts open in text editor instead of running

**Solution**: Right-click and select "Open in Command Prompt" or run from Command Prompt:
```cmd
cd path\to\ProLife
tools\quick-release.bat 2.1.0
```

**Issue**: Git Bash shows "bash: ./script.bat: cannot execute binary file"

**Solution**: Don't use .bat files in Git Bash. Use .sh files instead:
```bash
./tools/quick-release.sh 2.1.0
```

#### Linux/macOS-Specific

**Issue**: "bad interpreter: /bin/bash^M"

**Solution**: Script has Windows line endings. Fix with:
```bash
dos2unix tools/quick-release.sh
# or
sed -i 's/\r$//' tools/quick-release.sh
```

**Issue**: Script not found

**Solution**: Check you're in the repository root and script is executable:
```bash
ls -la tools/quick-release.sh
chmod +x tools/quick-release.sh
./tools/quick-release.sh 2.1.0
```

## Additional Resources

- **[BUILDING.md](BUILDING.md)** - Build instructions
- **[RELEASE_GUIDE.md](RELEASE_GUIDE.md)** - Release guide (English)
- **[RELEASE_GUIDE_RU.md](RELEASE_GUIDE_RU.md)** - Руководство по релизу (Russian)
- **[RELEASE_STRATEGY.md](RELEASE_STRATEGY.md)** - Release strategy
- **[tools/README.md](tools/README.md)** - Automation tools documentation
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - Contribution guidelines
- [Semantic Versioning](https://semver.org/) - Version numbering
- [Git Submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules) - Official Git documentation

---

**Document Version**: 1.0  
**Last Updated**: 2026-01-20  
**Maintained By**: ProLife Development Team
