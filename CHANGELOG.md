#  (2026-01-21)


### Bug Fixes

* remove MSVC toolset 14.3 from Windows build workflow ([e114953](https://github.com/ImagingTools/ProLife/commit/e1149531c3ab6df247774504c0263889b6f2a751))
* remove SUBMODULE_VERSIONS.md from .gitignore ([4a8a49d](https://github.com/ImagingTools/ProLife/commit/4a8a49d6c05144797b6d0a07c36dee61ac931ddf))
* use workflow ID instead of filename to avoid 404 errors ([83d2bee](https://github.com/ImagingTools/ProLife/commit/83d2beeeb307f084cec9bb80b5a7d27182c56437))

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
  - SBOM generation script (scripts/generate-sbom.sh) - supports CycloneDX 1.5 and SPDX 2.3 formats
  - CE Declaration template
  - Vulnerability management process documentation
  - Security acknowledgments file (SECURITY_ACKNOWLEDGMENTS.md)
  - Security contact information and reporting procedures
  - Comprehensive third-party components documentation (docs/compliance/THIRD_PARTY_COMPONENTS.md)
  - SPDX 2.3 support for SBOM generation (ISO/IEC 5962:2021)
  - Qt LGPL-3.0 license compliance documentation (docs/compliance/Qt_LGPL_Compliance.md)

### Features

* add comprehensive release automation and documentation ([7879bf0](https://github.com/ImagingTools/ProLife/commit/7879bf06d1bab71984d9fcbd423381726db4a157))
* add PERSONAL_ACCESS_TOKEN for private submodule authentication ([9850134](https://github.com/ImagingTools/ProLife/commit/98501341584fffbd56230afe0c510490986108a0))



# 1.0.0 (2026-01-16)



