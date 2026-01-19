# ProLife Release Strategy

## Overview

ProLife is a complex software product that integrates multiple dependent repositories as submodules. This document outlines the release strategy, versioning approach, and change management pipeline for ProLife and its dependencies.

## Repository Structure

### Main Repository
- **ProLife** - The main integration repository containing the final product

### Dependency Repositories (Submodules)
1. **Acf** - Advanced Computing Framework
2. **AcfSln** - ACF Solution files and configurations
3. **ImtCore** - Imaging Tools Core library
4. **Lisa** - Lisa application components
5. **Puma** - Puma application components
6. **Agentino** - Agentino application components

## Versioning Strategy

### Semantic Versioning

ProLife follows [Semantic Versioning 2.0.0](https://semver.org/) (MAJOR.MINOR.PATCH):

- **MAJOR** version: Incompatible API changes or major architectural changes
- **MINOR** version: New functionality in a backward-compatible manner
- **PATCH** version: Backward-compatible bug fixes

Example: `2.1.3`

### Version Tagging

All releases are tagged in Git using the format `v{MAJOR}.{MINOR}.{PATCH}`:
- Production releases: `v2.1.0`, `v2.1.1`
- Release candidates: `v2.1.0-rc1`, `v2.1.0-rc2`
- Beta releases: `v2.1.0-beta1`
- Alpha releases: `v2.1.0-alpha1`

### Submodule Version Pinning

Each ProLife release pins specific versions (commits or tags) of all dependency submodules. This ensures:
- Reproducible builds
- Stable releases
- Clear dependency tracking
- Simplified rollback procedures

## Branch Strategy

### Main Branches

- **main** (or **master**): Production-ready code, protected branch
  - Only accepts merges from release branches
  - Tagged with version numbers for releases
  
- **develop**: Integration branch for features
  - Contains the latest delivered development changes
  - Nightly builds run from this branch

- **release/v{MAJOR}.{MINOR}**: Release preparation branches
  - Created from develop when ready for release
  - Only bug fixes and release preparation commits
  - Merged to main when ready and tagged

### Supporting Branches

- **feature/{feature-name}**: New features
  - Branched from: develop
  - Merged back to: develop
  
- **bugfix/{bug-name}**: Non-critical bug fixes
  - Branched from: develop
  - Merged back to: develop
  
- **hotfix/{issue-name}**: Critical bug fixes for production
  - Branched from: main
  - Merged to: main and develop
  - Tagged with a new patch version

## Change Pipeline

### 1. Dependency Repository Changes

When changes are needed in a dependency repository (Acf, ImtCore, Lisa, Puma, Agentino, AcfSln):

```
┌─────────────────────────────────────────────────────────────┐
│ Dependency Repository (e.g., ImtCore)                       │
├─────────────────────────────────────────────────────────────┤
│ 1. Create feature branch                                    │
│ 2. Implement changes                                        │
│ 3. Run dependency's own tests                               │
│ 4. Create pull request                                      │
│ 5. Code review                                              │
│ 6. Merge to dependency's develop branch                     │
│ 7. Tag if needed (e.g., v1.2.3)                            │
└─────────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────────┐
│ ProLife Repository                                          │
├─────────────────────────────────────────────────────────────┤
│ 8. Update submodule reference in ProLife                    │
│ 9. Test integration                                         │
│ 10. Run ProLife test suite                                  │
│ 11. Commit submodule pointer update                         │
│ 12. Create PR in ProLife                                    │
│ 13. Integration testing & review                            │
│ 14. Merge to ProLife develop                                │
└─────────────────────────────────────────────────────────────┘
```

### 2. Direct ProLife Changes

For changes that only affect ProLife code:

1. Create feature branch from develop
2. Implement changes
3. Run tests
4. Create pull request
5. Code review
6. Merge to develop

### 3. Coordinated Multi-Repository Changes

When a change requires modifications across multiple repositories:

1. **Planning Phase**: Document dependencies between changes
2. **Implementation Phase**: 
   - Create feature branches in all affected repositories
   - Use consistent naming: `feature/cross-repo-feature-name`
3. **Testing Phase**:
   - Update ProLife to use feature branches of dependencies
   - Run integration tests
4. **Merge Phase**:
   - Merge dependency changes first
   - Update submodule pointers in ProLife
   - Merge ProLife changes

### 4. Hotfix Process

For critical production fixes:

```
┌─────────────────────────────────────────────────────────────┐
│ 1. Create hotfix branch from main                           │
│ 2. Fix the critical issue                                   │
│ 3. Update version to next patch (e.g., 2.1.3 → 2.1.4)      │
│ 4. Test thoroughly                                          │
│ 5. Merge to main                                            │
│ 6. Tag with new version                                     │
│ 7. Merge back to develop                                    │
│ 8. Deploy hotfix                                            │
└─────────────────────────────────────────────────────────────┘
```

## Release Process

### Preparing a Release

1. **Stabilization** (1-2 weeks before release):
   ```bash
   git checkout develop
   git pull
   git checkout -b release/v2.2.0
   ```

2. **Version Updates**:
   - Update version numbers in build files
   - Update CHANGELOG.md
   - Update documentation
   - Ensure all submodules are at stable commits/tags

3. **Testing**:
   - Run full test suite
   - Perform integration testing
   - Execute manual test scenarios
   - Performance testing
   - Security scanning

4. **Bug Fixes**:
   - Only critical and blocking bugs
   - No new features
   - All fixes go through normal PR process

### Creating a Release

5. **Finalization**:
   ```bash
   git checkout release/v2.2.0
   # Ensure everything is committed
   git checkout main
   git merge --no-ff release/v2.2.0
   git tag -a v2.2.0 -m "Release version 2.2.0"
   git push origin main --tags
   ```

6. **Back-merge**:
   ```bash
   git checkout develop
   git merge --no-ff release/v2.2.0
   git push origin develop
   git branch -d release/v2.2.0
   ```

7. **Build and Package**:
   - Trigger CI/CD pipeline
   - Create release artifacts
   - Generate release notes

8. **Deployment**:
   - Deploy to staging environment
   - Smoke tests
   - Deploy to production
   - Monitor for issues

### Post-Release

9. **Documentation**:
   - Publish release notes
   - Update user documentation
   - Announce release

10. **Monitoring**:
    - Monitor system health
    - Track error reports
    - Collect user feedback

## Dependency Update Strategy

### Regular Updates

- **Weekly**: Review dependency repositories for critical updates
- **Monthly**: Planned dependency updates during develop cycle
- **Per Release**: All dependencies reviewed and updated if needed

### Update Process

1. Check for updates in dependency repositories
   ```bash
   git submodule update --remote --merge
   ```

2. Review changes:
   ```bash
   cd 3rdParty/ImtCore
   git log HEAD@{1}..HEAD
   ```

3. Test integration:
   - Build ProLife with updated dependency
   - Run test suite
   - Perform smoke tests

4. Commit update:
   ```bash
   git add 3rdParty/ImtCore
   git commit -m "Update ImtCore to v1.5.0"
   ```

### Breaking Changes in Dependencies

When a dependency introduces breaking changes:

1. Create a feature branch in ProLife
2. Update submodule to new version
3. Adapt ProLife code to new API
4. Extensive testing
5. Document changes in CHANGELOG.md
6. Consider MAJOR version bump if breaking changes affect ProLife API

## Continuous Integration

### CI Pipeline Stages

1. **Build Stage**:
   - Checkout code with submodules
   - Build all components
   - Generate artifacts

2. **Test Stage**:
   - Unit tests
   - Integration tests
   - Code coverage analysis

3. **Quality Stage**:
   - Static code analysis
   - Security scanning
   - License compliance check

4. **Package Stage** (on main branch):
   - Create installation packages
   - Generate documentation
   - Archive artifacts

5. **Deploy Stage** (on tags):
   - Deploy to staging
   - Automated smoke tests
   - Deploy to production (manual approval)

### Submodule Handling in CI

```yaml
# Example CI configuration snippet
steps:
  - name: Checkout with submodules
    uses: actions/checkout@v3
    with:
      submodules: recursive
      token: ${{ secrets.GITHUB_TOKEN }}
  
  - name: Update submodules
    run: |
      git submodule update --init --recursive
```

## Version Control Best Practices

### Commit Messages

Follow the [Conventional Commits](https://www.conventionalcommits.org/) specification:

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

Types: `feat`, `fix`, `docs`, `style`, `refactor`, `perf`, `test`, `build`, `ci`, `chore`

Examples:
- `feat(client): add new authentication flow`
- `fix(server): resolve memory leak in connection handler`
- `chore(deps): update ImtCore to v1.5.0`

### Submodule Update Commits

When updating submodules, use descriptive commit messages:

```
chore(deps): update ImtCore to v1.5.0

- Includes performance improvements
- Fixes critical security issue CVE-2023-XXXX
- Adds support for new image format

Closes #123
```

## Rollback Procedures

### Rolling Back a Release

If a release needs to be rolled back:

1. **Immediate Action**:
   ```bash
   git checkout v2.1.2  # Previous stable version
   git tag v2.2.1-rollback
   git push origin v2.2.1-rollback
   ```

2. **Deploy Previous Version**:
   - Trigger deployment of rollback tag
   - Verify system stability

3. **Investigation**:
   - Identify root cause
   - Create hotfix if needed
   - Update test suite to prevent regression

### Rolling Back a Submodule

To revert a problematic submodule update:

```bash
cd 3rdParty/ImtCore
git checkout <previous-commit-hash>
cd ../..
git add 3rdParty/ImtCore
git commit -m "Revert ImtCore to previous stable version"
```

## Communication

### Release Announcements

- **Internal**: Slack/Teams notification, email to stakeholders
- **External**: Release notes on GitHub, documentation site, user mailing list

### Change Communication

- **Major Changes**: Advance notice, migration guides, deprecation warnings
- **Minor Changes**: Included in release notes
- **Patch Changes**: Brief mention in release notes

## Appendix

### Quick Reference Commands

```bash
# Clone with submodules
git clone --recursive https://github.com/ImagingTools/ProLife.git

# Initialize submodules after clone
git submodule update --init --recursive

# Update all submodules
git submodule update --remote --recursive

# Check submodule status
git submodule status

# Create a release
git checkout main
git merge --no-ff release/v2.2.0
git tag -a v2.2.0 -m "Release v2.2.0"
git push origin main --tags

# Update single submodule
cd 3rdParty/ImtCore
git fetch
git checkout v1.5.0
cd ../..
git add 3rdParty/ImtCore
git commit -m "chore(deps): update ImtCore to v1.5.0"
```

### Related Documentation

- `3rdParty/README.md` - Submodule management guide
- `BUILDING.md` - Build instructions
- `CHANGELOG.md` - Version history and changes
- `CONTRIBUTING.md` - Contribution guidelines

---

**Document Version**: 1.0  
**Last Updated**: 2026-01-19  
**Maintained By**: ProLife Development Team
