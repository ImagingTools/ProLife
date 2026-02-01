# Release Management Guide

This guide explains how to use the ProLife release preparation tools to manage versions and prepare releases.

## Overview

ProLife provides automated scripts to simplify the release preparation process:

- **`prepare-release.sh`** - Linux/macOS script
- **`prepare-release.bat`** - Windows script

These scripts help you:
- Manage submodule versions
- Create release branches
- Pin dependencies to specific versions
- Generate version reports
- Track submodule status

## Quick Start

### View Current Status

Check the current state of all submodules:

```bash
# Linux/macOS
./prepare-release.sh --status

# Windows
prepare-release.bat --status
```

### Initialize Submodules

If submodules are not initialized:

```bash
# Linux/macOS
./prepare-release.sh --init

# Windows
prepare-release.bat --init
```

## Creating a Release

Follow these steps to prepare a new release:

### Step 1: Check Current State

```bash
./prepare-release.sh --status
```

Review the current commit and tags for each submodule.

### Step 2: Create Release Branch

Create a release branch with the desired version:

```bash
./prepare-release.sh --create-release 2.1.0
```

This will:
- Create a new branch `release/v2.1.0`
- Switch to that branch
- Provide next steps

### Step 3: Update Submodule Versions

You have several options to update submodule versions:

#### Option A: Interactive Mode (Recommended)

```bash
./prepare-release.sh --interactive
```

This will:
- Show each submodule
- Display current version and available tags
- Prompt you to select a version for each submodule

#### Option B: Manual Update

Update a specific submodule to a specific version:

```bash
./prepare-release.sh --update-submodule 3rdParty/Acf v2.0.0
./prepare-release.sh --update-submodule 3rdParty/ImtCore v1.5.0
```

#### Option C: Update All to Latest

Update all submodules to their latest commits:

```bash
./prepare-release.sh --update-all
```

**Note:** Use this carefully as it may pull in unstable changes.

### Step 4: Update Version Numbers

Update version numbers in your code files:

- Update `CMakeLists.txt` or build configuration files
- Update any version constants in source code
- Update documentation with the new version

### Step 5: Update CHANGELOG.md

Edit `CHANGELOG.md` to document changes in this release:

```markdown
## [2.1.0] - 2026-01-20

### Added
- New feature X
- New feature Y

### Changed
- Improved Z

### Fixed
- Bug fix A

### Dependencies
- Updated Acf to v2.0.0
- Updated ImtCore to v1.5.0
```

### Step 6: Pin Submodules

Pin all submodules to their current commits:

```bash
./prepare-release.sh --pin-submodules
```

This ensures reproducible builds by locking submodules to specific commits.

### Step 7: Commit Changes

Commit all release preparation changes:

```bash
git status  # Review changes
git commit -m "Prepare release v2.1.0"
```

### Step 8: Push Release Branch

```bash
git push -u origin release/v2.1.0
```

### Step 9: Create Pull Request

Create a pull request from `release/v2.1.0` to `main` for final review.

### Step 10: Merge and Tag

After approval, merge to main and create a tag:

```bash
git checkout main
git merge --no-ff release/v2.1.0
git tag -a v2.1.0 -m "Release version 2.1.0"
git push origin main --tags
```

### Step 11: Back-merge to Develop

Merge release changes back to develop:

```bash
git checkout develop
git merge --no-ff release/v2.1.0
git push origin develop
```

## Script Reference

### Available Commands

#### `--status` or `-s`
Show current status of all submodules including commits, tags, and branches.

```bash
./prepare-release.sh --status
```

#### `--init`
Initialize all submodules (needed after fresh clone).

```bash
./prepare-release.sh --init
```

#### `--update-all`
Update all submodules to their latest commits on tracking branches.

```bash
./prepare-release.sh --update-all
```

#### `--pin-submodules`
Pin all submodules to their current commits and stage changes.

```bash
./prepare-release.sh --pin-submodules
```

#### `--create-release VERSION`
Create a new release branch with the specified version.

```bash
./prepare-release.sh --create-release 2.1.0
```

Version must follow semantic versioning: `MAJOR.MINOR.PATCH`

#### `--update-submodule PATH VERSION`
Update a specific submodule to a specific version (tag or commit).

```bash
./prepare-release.sh --update-submodule 3rdParty/Acf v2.0.0
./prepare-release.sh --update-submodule 3rdParty/ImtCore main
./prepare-release.sh --update-submodule 3rdParty/Puma abc123def
```

#### `--list-tags PATH`
List available tags for a specific submodule.

```bash
./prepare-release.sh --list-tags 3rdParty/Acf
```

#### `--interactive` or `-i`
Interactive mode to update submodule versions (Linux/macOS only).

```bash
./prepare-release.sh --interactive
```

This will walk through each submodule and prompt for version updates.

#### `--report` or `-r`
Generate a version report file (`SUBMODULE_VERSIONS.md`).

```bash
./prepare-release.sh --report
```

This creates a markdown file documenting the current state of all submodules.

#### `--help` or `-h`
Show help message with all available options.

```bash
./prepare-release.sh --help
```

## Common Workflows

### Hotfix Release

For critical production fixes:

```bash
# From main branch
git checkout main
git checkout -b hotfix/v2.0.1

# Fix the issue
# ... make changes ...

# Update version
# Edit version files

# Commit
git commit -am "Hotfix: critical bug fix"

# Merge to main
git checkout main
git merge --no-ff hotfix/v2.0.1
git tag -a v2.0.1 -m "Hotfix release 2.0.1"
git push origin main --tags

# Merge to develop
git checkout develop
git merge --no-ff hotfix/v2.0.1
git push origin develop

# Clean up
git branch -d hotfix/v2.0.1
```

### Update Single Dependency

To update just one submodule:

```bash
# Check available versions
./prepare-release.sh --list-tags 3rdParty/Acf

# Update to specific version
./prepare-release.sh --update-submodule 3rdParty/Acf v2.0.0

# Test the change
# Run builds and tests

# Commit
git commit -m "chore(deps): update Acf to v2.0.0"
```

### Roll Back Submodule

To revert a submodule to a previous version:

```bash
cd 3rdParty/Acf
git checkout v1.9.0
cd ../..
git add 3rdParty/Acf
git commit -m "Revert Acf to v1.9.0"
```

Or use the script:

```bash
./prepare-release.sh --update-submodule 3rdParty/Acf v1.9.0
git commit -m "Revert Acf to v1.9.0"
```

### Generate Version Report

Create a snapshot of current submodule versions:

```bash
./prepare-release.sh --report
```

This generates `SUBMODULE_VERSIONS.md` which you can:
- Include in release notes
- Archive for record keeping
- Share with team members

## Best Practices

### Version Selection

1. **Use tagged versions** when available (e.g., `v2.0.0`)
2. **Use stable branches** for dependencies in active development (e.g., `main`, `master`)
3. **Avoid using commit hashes** unless necessary for specific fixes
4. **Test integration** after updating any submodule

### Testing

Before finalizing a release:

1. **Build the project** to ensure no compilation errors
2. **Run test suites** to catch regressions
3. **Perform integration tests** to verify submodule compatibility
4. **Test on multiple platforms** if applicable

### Documentation

Always document submodule updates:

```bash
# Good commit message
git commit -m "chore(deps): update Acf to v2.0.0

- Includes performance improvements
- Fixes memory leak in connection handler
- Adds support for new protocol

Closes #123"
```

### Communication

- **Notify team members** before updating submodules
- **Announce** major version updates
- **Document breaking changes** in CHANGELOG.md
- **Update** dependent documentation

## Troubleshooting

### Submodules Not Initialized

**Problem:** Submodule directories are empty

**Solution:**
```bash
./prepare-release.sh --init
```

### Authentication Errors

**Problem:** Cannot access private submodules

**Solution:**

For SSH:
```bash
git config --global url."git@github.com:".insteadOf "https://github.com/"
ssh-add ~/.ssh/id_rsa
```

For HTTPS with token:
```bash
git config --global credential.helper store
```

### Detached HEAD State

**Problem:** Submodule is in detached HEAD state

**Solution:**
```bash
cd 3rdParty/<submodule>
git checkout main  # or master
cd ../..
./prepare-release.sh --pin-submodules
```

### Merge Conflicts in Submodules

**Problem:** Git reports conflicts in submodule pointers

**Solution:**
```bash
# Accept theirs
git checkout --theirs 3rdParty/<submodule>
git add 3rdParty/<submodule>

# Or accept ours
git checkout --ours 3rdParty/<submodule>
git add 3rdParty/<submodule>

# Or manually select version
cd 3rdParty/<submodule>
git checkout <desired-version>
cd ../..
git add 3rdParty/<submodule>
```

### Script Execution Issues

**Problem:** Permission denied on Linux/macOS

**Solution:**
```bash
chmod +x prepare-release.sh
```

**Problem:** Line ending issues (CRLF vs LF)

**Solution:**
```bash
# Convert to Unix line endings
dos2unix prepare-release.sh

# Or configure git
git config core.autocrlf input
```

## Integration with CI/CD

### GitHub Actions Example

```yaml
name: Release

on:
  push:
    branches:
      - 'release/**'

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
        with:
          submodules: recursive
      
      - name: Verify submodules
        run: |
          ./prepare-release.sh --status
          ./prepare-release.sh --report
          
      - name: Build
        run: |
          mkdir build && cd build
          cmake ..
          cmake --build . --config Release
      
      - name: Test
        run: |
          cd build
          ctest --config Release
```

### GitLab CI Example

```yaml
release:
  stage: deploy
  script:
    - git submodule update --init --recursive
    - ./prepare-release.sh --status
    - ./prepare-release.sh --report
    - mkdir build && cd build
    - cmake ..
    - cmake --build . --config Release
    - ctest --config Release
  only:
    - /^release\/.*$/
```

## Additional Resources

- [RELEASE_STRATEGY.md](RELEASE_STRATEGY.md) - Complete release strategy documentation
- [BUILDING.md](BUILDING.md) - Build instructions
- [3rdParty/README.md](3rdParty/README.md) - Submodule management guide
- [Semantic Versioning](https://semver.org/) - Version numbering specification
- [Keep a Changelog](https://keepachangelog.com/) - Changelog format

## Getting Help

If you encounter issues:

1. Check this documentation
2. Review error messages carefully
3. Check submodule status: `./prepare-release.sh --status`
4. Consult [RELEASE_STRATEGY.md](RELEASE_STRATEGY.md)
5. Contact the development team

---

**Last Updated:** 2026-01-20  
**Maintained By:** ProLife Development Team
