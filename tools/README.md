# ProLife Release Automation Tools

This directory contains automated tools for release preparation and validation, available for both Linux/macOS (`.sh` scripts) and Windows (`.bat` scripts).

## Cross-Platform Support

All tools are available in two versions:
- **Linux/macOS**: `.sh` (Bash scripts)
- **Windows**: `.bat` (Batch scripts)

Both versions provide identical functionality and can be used interchangeably based on your operating system.

## Tools Overview

### validate-release.sh / validate-release.bat

**Purpose:** Comprehensive validation and automation for release preparation.

**Features:**
- Validates submodule initialization
- Checks submodule versions (tags vs branches vs commits)
- Detects uncommitted changes
- Validates CHANGELOG.md updates
- Verifies version in CMakeLists.txt
- Auto-updates submodules to latest tagged versions
- Generates detailed submodule reports
- Provides release summaries

**Usage:**

*Linux/macOS:*
```bash
# Full validation
./tools/validate-release.sh validate v2.1.0

# Auto-update all submodules to latest tags
./tools/validate-release.sh auto-update

# Pin submodules with validation
./tools/validate-release.sh pin

# Generate version report
./tools/validate-release.sh report SUBMODULE_VERSIONS.md

# Show release summary
./tools/validate-release.sh summary v2.1.0

# Help
./tools/validate-release.sh help
```

*Windows:*
```cmd
REM Full validation
tools\validate-release.bat validate v2.1.0

REM Auto-update all submodules to latest tags
tools\validate-release.bat auto-update

REM Pin submodules with validation
tools\validate-release.bat pin

REM Generate version report
tools\validate-release.bat report SUBMODULE_VERSIONS.md

REM Show release summary
tools\validate-release.bat summary v2.1.0

REM Help
tools\validate-release.bat help
```

**When to use:**
- Before starting release preparation
- After updating submodules
- Before committing release changes
- During CI/CD pipeline

### quick-release.sh / quick-release.bat

**Purpose:** One-command automated release preparation.

**Features:**
- Creates release branch automatically
- Initializes submodules
- Auto-updates submodules to latest tagged versions
- Builds and tests the project (optional)
- Updates version in CMakeLists.txt
- Generates submodule version report
- Pins all submodules
- Runs final validation
- Provides clear next steps

**Usage:**

*Linux/macOS:*
```bash
# Full automated preparation
./tools/quick-release.sh 2.1.0

# Skip build and tests (faster)
./tools/quick-release.sh 2.1.0 --skip-tests
```

*Windows:*
```cmd
REM Full automated preparation
tools\quick-release.bat 2.1.0

REM Skip build and tests (faster)
tools\quick-release.bat 2.1.0 --skip-tests
```

**When to use:**
- Starting a new release from scratch
- Quick release preparation
- Automated release workflows

**What it does NOT do (you must do manually):**
- Update CHANGELOG.md with release notes
- Create the actual GitHub release
- Merge the release branch

### set-submodule-versions.sh / set-submodule-versions.bat

**Purpose:** Interactive submodule version management tool.

**Features:**
- Auto-update all submodules to latest tagged versions
- Interactively select versions for each submodule
- Set specific version for one submodule
- Show current submodule versions

**Usage:**

*Linux/macOS:*
```bash
./tools/set-submodule-versions.sh
```

*Windows:*
```cmd
tools\set-submodule-versions.bat
```

**When to use:**
- Need to select specific versions for submodules
- Want an interactive interface for version management
- Need to review available versions before updating
- Preparing custom version combinations

## Workflow Comparison

### Manual Workflow (Traditional)

*Linux/macOS:*
```bash
# 10+ manual steps
git checkout -b release/v2.1.0
git submodule update --init --recursive
cd 3rdParty/Acf && git checkout v2.0.0 && cd ../..
cd 3rdParty/ImtCore && git checkout v1.5.0 && cd ../..
# ... repeat for all submodules
vim Build/CMake/CMakeLists.txt  # Update version
vim CHANGELOG.md  # Update changelog
git add .
git submodule foreach 'git rev-parse HEAD'
git commit -m "Prepare release v2.1.0"
# Build and test...
git push
```

*Windows:*
```cmd
REM 10+ manual steps
git checkout -b release/v2.1.0
git submodule update --init --recursive
cd 3rdParty\Acf && git checkout v2.0.0 && cd ..\..
cd 3rdParty\ImtCore && git checkout v1.5.0 && cd ..\..
REM ... repeat for all submodules
notepad Build\CMake\CMakeLists.txt  REM Update version
notepad CHANGELOG.md  REM Update changelog
git add .
git submodule foreach "git rev-parse HEAD"
git commit -m "Prepare release v2.1.0"
REM Build and test...
git push
```

### Automated Workflow (With Tools)

*Linux/macOS:*
```bash
# 1 command + manual CHANGELOG update
./tools/quick-release.sh 2.1.0
vim CHANGELOG.md
git add . && git commit -m "chore: prepare release v2.1.0"
git push -u origin release/v2.1.0
```

*Windows:*
```cmd
REM 1 command + manual CHANGELOG update
tools\quick-release.bat 2.1.0
notepad CHANGELOG.md
git add . && git commit -m "chore: prepare release v2.1.0"
git push -u origin release/v2.1.0
```

## Platform-Specific Instructions

### Linux/macOS Setup

**Make scripts executable (first time only):**
```bash
chmod +x tools/quick-release.sh
chmod +x tools/validate-release.sh
chmod +x tools/set-submodule-versions.sh
```

**Running scripts:**
```bash
./tools/quick-release.sh 2.1.0
./tools/validate-release.sh validate
./tools/set-submodule-versions.sh
```

### Windows Setup

**No setup required!** Just run the scripts directly:

*Command Prompt:*
```cmd
tools\quick-release.bat 2.1.0
tools\validate-release.bat validate
tools\set-submodule-versions.bat
```

*PowerShell:*
```powershell
.\tools\quick-release.bat 2.1.0
.\tools\validate-release.bat validate
.\tools\set-submodule-versions.bat
```

*Git Bash (if installed):*
```bash
# Can use either .sh or .bat scripts
./tools/quick-release.sh 2.1.0
# or
./tools/quick-release.bat 2.1.0
```

**Recommendation:** Use `.bat` scripts in Command Prompt or PowerShell for best Windows compatibility.

## Tool Selection Guide

| Task | Tool | Linux/macOS Command | Windows Command |
|------|------|---------------------|-----------------|
| Full release automation | quick-release | `./tools/quick-release.sh 2.1.0` | `tools\quick-release.bat 2.1.0` |
| Just validate | validate-release | `./tools/validate-release.sh validate v2.1.0` | `tools\validate-release.bat validate v2.1.0` |
| Just update submodules | validate-release | `./tools/validate-release.sh auto-update` | `tools\validate-release.bat auto-update` |
| Interactive version selection | set-submodule-versions | `./tools/set-submodule-versions.sh` | `tools\set-submodule-versions.bat` |
| Generate report only | validate-release | `./tools/validate-release.sh report` | `tools\validate-release.bat report` |
| Check current state | validate-release | `./tools/validate-release.sh validate` | `tools\validate-release.bat validate` |
| Manual control | prepare-release | `./prepare-release.sh --status` | `prepare-release.bat --status` |

## Integration with GitHub Actions

Both tools integrate seamlessly with the GitHub Actions workflows:

### Local Preparation → GitHub Actions Release

*Linux/macOS:*
```bash
# 1. Local preparation
./tools/quick-release.sh 2.1.0
vim CHANGELOG.md
git add . && git commit -m "chore: prepare release v2.1.0"
git push -u origin release/v2.1.0

# 2. GitHub Actions (in web UI)
# Go to Actions → Release Preparation → Run workflow
# - Branch: release/v2.1.0
# - Target tag: v2.1.0
# - Base tag: v2.0.0
# - Dry run: false
```

*Windows:*
```cmd
REM 1. Local preparation
tools\quick-release.bat 2.1.0
notepad CHANGELOG.md
git add . && git commit -m "chore: prepare release v2.1.0"
git push -u origin release/v2.1.0

REM 2. GitHub Actions (in web UI)
REM Go to Actions → Release Preparation → Run workflow
REM - Branch: release/v2.1.0
REM - Target tag: v2.1.0
REM - Base tag: v2.0.0
REM - Dry run: false
```

### Fully Automated (GitHub Actions only)

The workflow now automatically:
1. Checks out code with submodules
2. Runs validation
3. Generates submodule report
4. Updates changelog
5. Updates version
6. Commits changes
7. Creates tag and release

## Best Practices

### For Developers

1. **Always validate first:**
   ```bash
   ./tools/validate-release.sh validate
   ```

2. **Use quick-release for new releases:**
   ```bash
   ./tools/quick-release.sh 2.1.0
   ```

3. **Review changes before committing:**
   ```bash
   git status
   git diff
   ```

4. **Test after submodule updates:**
   - Build should succeed
   - All tests should pass
   - Manual smoke tests recommended

### For CI/CD

1. **Add validation to PR checks:**
   ```yaml
   - name: Validate release prep
     run: ./tools/validate-release.sh validate
   ```

2. **Generate reports in CI:**
   ```yaml
   - name: Generate submodule report
     run: ./tools/validate-release.sh report
   ```

3. **Archive reports:**
   ```yaml
   - name: Upload report
     uses: actions/upload-artifact@v4
     with:
       name: submodule-versions
       path: SUBMODULE_VERSIONS.md
   ```

## Common Issues and Solutions

### Issue: Permission Denied

*Linux/macOS:*
```bash
# Solution
chmod +x tools/validate-release.sh
chmod +x tools/quick-release.sh
chmod +x tools/set-submodule-versions.sh
```

*Windows:*
```
No action needed - Windows doesn't require execute permissions
```

### Issue: Submodules Not Initialized

*Linux/macOS:*
```bash
# Solution
git submodule update --init --recursive
# or
./tools/quick-release.sh 2.1.0  # Does this automatically
```

*Windows:*
```cmd
REM Solution
git submodule update --init --recursive
REM or
tools\quick-release.bat 2.1.0  REM Does this automatically
```

### Issue: Validation Fails

*Linux/macOS:*
```bash
# Solution: Run validation to see specific errors
./tools/validate-release.sh validate v2.1.0

# Common fixes:
# - Commit or stash uncommitted changes
# - Initialize submodules
# - Update CHANGELOG.md
# - Fix version in CMakeLists.txt
```

*Windows:*
```cmd
REM Solution: Run validation to see specific errors
tools\validate-release.bat validate v2.1.0

REM Common fixes:
REM - Commit or stash uncommitted changes
REM - Initialize submodules
REM - Update CHANGELOG.md
REM - Fix version in CMakeLists.txt
```

### Issue: Build Fails After Submodule Update

*Linux/macOS:*
```bash
# Solution: This indicates incompatibility
# Option 1: Rollback problematic submodule
./prepare-release.sh --update-submodule 3rdParty/Acf v1.9.0

# Option 2: Fix compatibility issues in code
# ... make necessary changes ...

# Option 3: Use a different version
./prepare-release.sh --list-tags 3rdParty/Acf
./prepare-release.sh --update-submodule 3rdParty/Acf v2.0.1
```

*Windows:*
```cmd
REM Solution: This indicates incompatibility
REM Option 1: Rollback problematic submodule
prepare-release.bat --update-submodule 3rdParty\Acf v1.9.0

REM Option 2: Fix compatibility issues in code
REM ... make necessary changes ...

REM Option 3: Use a different version
prepare-release.bat --list-tags 3rdParty\Acf
prepare-release.bat --update-submodule 3rdParty\Acf v2.0.1
```

## Advanced Usage

### Custom Validation Rules

Edit `validate-release.sh` to add project-specific validation:

```bash
# Add to validate-release.sh
validate_custom_requirements() {
    print_header "Custom Project Validation"
    
    # Example: Check for required files
    if [ ! -f "docs/RELEASE_NOTES.md" ]; then
        print_error "Missing RELEASE_NOTES.md"
    fi
    
    # Example: Check version consistency
    # ... add your checks ...
}
```

### Integration with Other Tools

```bash
# Use with conventional commits
npm install -g conventional-changelog-cli
conventional-changelog -p angular -i CHANGELOG.md -s -r 0

# Combine with your tools
./tools/quick-release.sh 2.1.0
./your-custom-script.sh
git commit --amend
```

### Automated Version Bumping

```bash
# Semantic version bumping
current_version=$(grep "project(ProLifeAll VERSION" Build/CMake/CMakeLists.txt | sed -n 's/.*VERSION \([0-9.]*\).*/\1/p')

# Bump patch version
IFS='.' read -r major minor patch <<< "$current_version"
new_version="$major.$minor.$((patch + 1))"

./tools/quick-release.sh "$new_version"
```

## Security Considerations

### Token Management

Never commit tokens or credentials. The scripts use:
- Git configuration (safe)
- Environment variables (when needed)
- GitHub Actions secrets (in workflows)

### Submodule URLs

Ensure all submodule URLs are correct and trusted:

```bash
# Check submodule URLs
git config --file .gitmodules --get-regexp url

# Update if needed
git config --file .gitmodules submodule.3rdParty/Acf.url https://github.com/ImagingTools/Acf.git
```

### Validation Before Release

Always run validation before creating public releases:

```bash
./tools/validate-release.sh validate v2.1.0
```

## Troubleshooting

### Debug Mode

Enable debug output:

```bash
# Bash debug mode
bash -x ./tools/validate-release.sh validate v2.1.0

# Verbose Git
GIT_TRACE=1 ./tools/validate-release.sh auto-update
```

### Dry Run

Test without making changes:

```bash
# In GitHub Actions, use dry_run: true
# For local testing, review changes before committing
./tools/quick-release.sh 2.1.0 --skip-tests
git status  # Review
git diff    # Review changes
# Don't commit if not satisfied
git reset --hard HEAD  # Undo if needed
```

## Contributing

To improve these tools:

1. Test your changes thoroughly
2. Update this README
3. Add examples
4. Document any breaking changes

## Support

For help:
1. Check this README
2. Run with `--help` or `help` flag
3. Check script comments
4. Review related documentation:
   - **[RELEASE_PROCESS.md](../RELEASE_PROCESS.md)** - Comprehensive release process guide
   - **[RELEASE_GUIDE_RU.md](../RELEASE_GUIDE_RU.md)** - Russian release guide
   - **[RELEASE_GUIDE.md](../RELEASE_GUIDE.md)** - English release guide
5. Contact the development team

## Platform-Specific Troubleshooting

### Windows Issues

**Problem:** Scripts open in text editor instead of running

**Solution:** Right-click script and select "Open with Command Prompt" or run from Command Prompt:
```cmd
cd path\to\ProLife
tools\quick-release.bat 2.1.0
```

**Problem:** Git Bash shows "cannot execute binary file"

**Solution:** In Git Bash, use `.sh` scripts, not `.bat`:
```bash
./tools/quick-release.sh 2.1.0
```

**Problem:** Path not found errors

**Solution:** Windows uses backslashes. Make sure to use:
```cmd
tools\validate-release.bat
```
Not:
```cmd
tools/validate-release.bat
```

### Linux/macOS Issues

**Problem:** "bad interpreter: /bin/bash^M"

**Solution:** Script has Windows line endings. Fix with:
```bash
dos2unix tools/*.sh
# or
sed -i 's/\r$//' tools/*.sh
```

**Problem:** Script not found

**Solution:** Ensure you're in repo root and scripts are executable:
```bash
pwd  # Should show ProLife directory
ls -la tools/
chmod +x tools/*.sh
```

**Problem:** Cannot find git command

**Solution:** Install git:
```bash
# Ubuntu/Debian
sudo apt-get install git

# macOS
brew install git
```

---

**Last Updated:** 2026-01-20  
**Maintained By:** ProLife Development Team
