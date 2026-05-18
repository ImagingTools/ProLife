# ProLife Third-Party Components

This document provides a comprehensive list of all third-party components used in ProLife, as required for EU Cyber Resilience Act (CRA) compliance and transparency.

## Overview

ProLife integrates various open-source and proprietary third-party components to provide its comprehensive medical imaging and laboratory information management functionality. This document is part of the Software Bill of Materials (SBOM) documentation and is maintained to ensure compliance with the EU CRA requirements for transparency and license compliance.

## Component Categories

### 1. Core Framework Components

#### Qt Framework
- **Version:** 5.x or 6.x (configurable)
- **License:** LGPL-3.0-only (or commercial license)
- **Purpose:** Cross-platform application framework for UI and core functionality
- **Components Used:**
  - Qt Core - Core non-GUI functionality
  - Qt Widgets - Desktop UI widgets
  - Qt Quick/QuickWidgets - QML-based UI
  - Qt GUI - GUI base classes
  - Qt XML - XML processing
  - Qt Network - Network programming
  - Qt SVG - SVG rendering
  - Qt SQL - Database integration
  - Qt WebSockets - WebSocket protocol support
  - Qt QML - QML engine
  - Qt Concurrent - Multi-threading support
  - Qt QuickControls2 - QML controls
  - Qt Core5Compat (Qt6 only) - Qt5 compatibility
- **Website:** https://www.qt.io
- **Usage:** Primary application framework for desktop client and UI components

### 2. Build System Components

#### CMake
- **Version:** 3.26+
- **License:** BSD-3-Clause
- **Purpose:** Cross-platform build system generator
- **Website:** https://cmake.org
- **Usage:** Primary build system for compiling ProLife from source

#### QMake
- **Version:** Included with Qt
- **License:** Same as Qt
- **Purpose:** Alternative Qt-based build system
- **Usage:** Secondary build system option for Qt-specific builds

### 3. Security and Cryptography

#### OpenSSL
- **Version:** 3.0.x
- **License:** Apache-2.0
- **Purpose:** Cryptography and SSL/TLS toolkit
- **Website:** https://www.openssl.org
- **Usage:** 
  - Secure communications (HTTPS, TLS)
  - Data encryption at rest
  - Certificate management
  - Cryptographic operations

### 4. Server Components

#### nginx
- **Version:** 1.25.x (approximate)
- **License:** BSD-2-Clause
- **Purpose:** HTTP and reverse proxy server
- **Website:** https://nginx.org
- **Location:** `Install/ProLifeServer/nginx/`
- **Usage:**
  - Web server for ProLife Server
  - Reverse proxy for backend services
  - Load balancing
  - Static file serving

### 5. Testing Components

#### Playwright
- **Version:** 1.52.0
- **License:** Apache-2.0
- **Purpose:** Browser automation framework for end-to-end testing
- **Website:** https://playwright.dev
- **Location:** `Tests/frontend/package.json`
- **Usage:** Frontend automated testing and UI validation

### 6. ImagingTools Proprietary Components

These are internal components developed and maintained by ImagingTools GmbH, distributed as Git submodules:

#### Acf (Advanced Computing Framework)
- **Type:** Internal Library
- **License:** ImagingTools Proprietary
- **Repository:** https://github.com/ImagingTools/Acf
- **Location:** `3rdParty/Acf/`
- **Purpose:** Core computing framework for ProLife
- **Access:** Public repository

#### AcfSln (ACF Solution)
- **Type:** Internal Configuration
- **License:** ImagingTools Proprietary
- **Repository:** https://github.com/ImagingTools/AcfSln
- **Location:** `3rdParty/AcfSln/`
- **Purpose:** ACF solution files and configurations
- **Access:** Public repository

#### ImtCore (Imaging Tools Core)
- **Type:** Internal Library
- **License:** ImagingTools Proprietary
- **Repository:** https://github.com/ImagingTools/ImtCore (private)
- **Location:** `3rdParty/ImtCore/`
- **Purpose:** Core imaging library and utilities
- **Access:** Private repository (requires authentication)

#### Lisa
- **Type:** Internal Application Component
- **License:** ImagingTools Proprietary
- **Repository:** https://github.com/ImagingTools/Lisa (private)
- **Location:** `3rdParty/Lisa/`
- **Purpose:** Lisa application components
- **Access:** Private repository (requires authentication)

#### Puma
- **Type:** Internal Application Component
- **License:** ImagingTools Proprietary
- **Repository:** https://github.com/ImagingTools/Puma (private)
- **Location:** `3rdParty/Puma/`
- **Purpose:** Puma application components
- **Access:** Private repository (requires authentication)

#### Agentino
- **Type:** Internal Application Component
- **License:** ImagingTools Proprietary
- **Repository:** https://github.com/ImagingTools/Agentino (private)
- **Location:** `3rdParty/Agentino/`
- **Purpose:** Agentino application components
- **Access:** Private repository (requires authentication)

## License Summary

### Open Source Licenses

| Component | License | Category | Commercial Use |
|-----------|---------|----------|----------------|
| Qt Framework | LGPL-3.0-only | Weak Copyleft | Allowed with dynamic linking |
| CMake | BSD-3-Clause | Permissive | Allowed |
| OpenSSL | Apache-2.0 | Permissive | Allowed |
| nginx | BSD-2-Clause | Permissive | Allowed |
| Playwright | Apache-2.0 | Permissive | Allowed (dev dependency) |

### Proprietary Licenses

| Component | License | Owner |
|-----------|---------|-------|
| ProLife | ImagingTools Commercial License | ImagingTools GmbH |
| Acf | ImagingTools Proprietary | ImagingTools GmbH |
| AcfSln | ImagingTools Proprietary | ImagingTools GmbH |
| ImtCore | ImagingTools Proprietary | ImagingTools GmbH |
| Lisa | ImagingTools Proprietary | ImagingTools GmbH |
| Puma | ImagingTools Proprietary | ImagingTools GmbH |
| Agentino | ImagingTools Proprietary | ImagingTools GmbH |

## License Compliance

### LGPL-3.0 Compliance (Qt Framework)

ProLife complies with the LGPL-3.0 license for Qt by:
- Using Qt as a dynamically linked library (not statically linked)
- Not modifying Qt source code
- Allowing end users to replace Qt libraries
- Providing information about Qt usage in documentation
- Commercial Qt license is also available for customers who prefer it

**Detailed LGPL Compliance Documentation:**  
See [Qt_LGPL_Compliance.md](Qt_LGPL_Compliance.md) for comprehensive documentation on:
- Dynamic linking verification
- User modification rights
- Installation information for replacing Qt
- Source code availability
- Complete LGPL-3.0 compliance checklist

### Attribution Requirements

The following components require attribution in documentation:
- **Qt Framework**: "This application uses Qt, a cross-platform application framework. Qt is copyright The Qt Company Ltd."
- **OpenSSL**: "This product includes software developed by the OpenSSL Project for use in the OpenSSL Toolkit (https://www.openssl.org/)"
- **nginx**: "This product includes nginx, copyright F5, Inc."

## Vulnerability Tracking

All third-party components are tracked for known vulnerabilities:

- **Automated Scanning**: Daily scans using dependency analysis tools
- **CVE Monitoring**: Active monitoring of CVE databases for all components
- **Update Policy**: Security updates applied according to severity timelines (see SECURITY.md)
- **SBOM Updates**: Software Bill of Materials updated with each release

## Component Update Policy

### Regular Updates
- **Qt Framework**: Updated to latest stable LTS version annually
- **OpenSSL**: Security patches applied within 14 days of release
- **nginx**: Updated to stable versions with security fixes
- **CMake**: Updated as needed for new features and compatibility
- **Playwright**: Updated regularly for testing improvements

### Submodule Updates
- **ImagingTools components**: Updated according to release strategy (see RELEASE_STRATEGY.md)
- Version control via Git submodules
- Synchronized updates across all ProLife installations

## Documentation and Resources

### Component Documentation
- **Qt**: https://doc.qt.io/
- **CMake**: https://cmake.org/documentation/
- **OpenSSL**: https://www.openssl.org/docs/
- **nginx**: https://nginx.org/en/docs/
- **Playwright**: https://playwright.dev/docs/intro

### ProLife Specific Documentation
- **SBOM**: `docs/sbom/` - Complete Software Bill of Materials
- **Security Policy**: `SECURITY.md` - Vulnerability reporting and security updates
- **Building Guide**: `BUILDING.md` - Build instructions including dependencies
- **3rdParty README**: `3rdParty/README.md` - Submodule management guide

## Compliance and Auditing

### EU CRA Compliance
- **Transparency**: Complete list of all components maintained
- **SBOM**: Machine-readable SBOM in CycloneDX format
- **License Tracking**: All licenses identified and complied with
- **Update Records**: Component update history maintained
- **Vulnerability Management**: Active monitoring and patching process

### Audit Trail
- **Component Versions**: Tracked in SBOM for each release
- **License Changes**: Monitored and documented
- **Security Patches**: Applied and documented in CHANGELOG
- **Compliance Reviews**: Quarterly reviews of all components

## Contact Information

For questions about third-party components:

- **License Compliance**: compliance@imagingtools.com
- **Security Issues**: security@imagingtools.com
- **Technical Support**: support@imagingtools.com
- **Component Updates**: See GitHub repository for latest information

## Version History

| Version | Date       | Changes |
|---------|------------|---------|
| 1.0     | 2026-01-19 | Initial comprehensive third-party component documentation |

---

**Note:** This document is maintained as part of EU CRA compliance requirements. It must be reviewed and updated with each release and whenever component versions or licenses change.

**Last Updated:** 2026-01-19  
**Document Owner:** ImagingTools GmbH Compliance Team
