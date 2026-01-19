# Changelog

All notable changes to ProLife will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Git submodules for all dependency repositories (Acf, AcfSln, ImtCore, Lisa, Puma, Agentino)
- Comprehensive release strategy documentation (RELEASE_STRATEGY.md)
- Detailed build instructions (BUILDING.md)
- Submodule management guide (3rdParty/README.md)
- Environment setup scripts (setup-environment.sh, setup-environment.bat)
- Root README.md with project overview and quick start guide
- CMake configuration updates to support submodule paths
- Automatic fallback to submodule paths when environment variables are not set
- **EU Cyber Resilience Act (CRA) compliance support**
  - SECURITY.md with vulnerability disclosure policy
  - EU CRA compliance documentation (docs/compliance/eu-cra/)
  - Software Bill of Materials (SBOM) framework (docs/sbom/)
  - SBOM generation script (scripts/generate-sbom.sh)
  - CE Declaration template
  - Vulnerability management process documentation
  - Security acknowledgments file (SECURITY_ACKNOWLEDGMENTS.md)
  - Security contact information and reporting procedures

### Changed
- Dependency management migrated from environment variables to Git submodules
- Build system now supports both environment variable and submodule-based paths
- Improved project structure for better dependency tracking

### Documentation
- Added comprehensive documentation for release management
- Added dependency change pipeline documentation
- Added CI/CD integration recommendations
- Added troubleshooting guides for common build issues

## Previous Versions

Historical changes from before the submodule migration are tracked in the respective component repositories.

---

## Release Notes Template

Use this template for future releases:

## [X.Y.Z] - YYYY-MM-DD

### Added
- New features

### Changed
- Changes in existing functionality

### Deprecated
- Soon-to-be removed features

### Removed
- Removed features

### Fixed
- Bug fixes

### Security
- Vulnerability fixes

### Dependencies
- Submodule updates and changes

---

[Unreleased]: https://github.com/ImagingTools/ProLife/compare/main...develop
