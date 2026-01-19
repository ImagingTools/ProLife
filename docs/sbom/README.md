# Software Bill of Materials (SBOM)

## Overview

This directory contains Software Bill of Materials (SBOM) files for ProLife releases, as required by the EU Cyber Resilience Act (CRA).

**Important:** ProLife is distributed under a **commercial license** (ImagingTools Enterprise License Agreement). The SBOM includes all components used in ProLife, including:
- ProLife proprietary components (commercial license)
- Third-party commercial components (various commercial licenses)
- Third-party open-source components (LGPL, Apache, BSD, etc.)

All component licenses are clearly identified in the SBOM for compliance and transparency.

## What is an SBOM?

A Software Bill of Materials (SBOM) is a comprehensive inventory of all components, libraries, and dependencies used in a software product. It provides transparency into the software supply chain and enables:

- Vulnerability tracking and management
- License compliance verification
- Supply chain risk assessment
- Incident response and remediation

## SBOM Format

ProLife SBOMs are provided in **CycloneDX 1.5** format (JSON), which is:

- Industry-standard format supported by the EU CRA
- Machine-readable for automated processing
- Extensible and comprehensive
- Compatible with vulnerability scanning tools

## SBOM Files

Each release of ProLife includes an SBOM file:

- **Format:** CycloneDX 1.5 JSON
- **Naming:** `sbom-<version>.json` (e.g., `sbom-1.0.0.json`)
- **Location:** This directory (`docs/sbom/`)
- **Signature:** Each SBOM is signed for verification

### Current Release

- **Latest SBOM:** `sbom-latest.json` (symbolic link to current version)
- **Version:** [Generated for each release]

## SBOM Contents

Each ProLife SBOM includes:

### 1. Component Information
- Component name and version
- Package URL (purl)
- Supplier/author information
- Component type (library, application, framework, etc.)
- Source code repository URL

### 2. Dependency Information
- Direct dependencies
- Transitive dependencies
- Dependency relationships
- Dependency graph

### 3. License Information
- License identifier (SPDX)
- License text reference
- License compliance status
- Multiple licenses (where applicable)

### 4. Vulnerability Information
- Known CVEs affecting components
- Vulnerability severity (CVSS score)
- Affected versions
- Fixed versions (if available)
- Vulnerability status (open, resolved, false positive)

### 5. Build Information
- Build timestamp
- Build environment
- Build tools and versions
- Git commit SHA

## Generating an SBOM

### Prerequisites

Install CycloneDX CLI tool:

```bash
# Using npm
npm install -g @cyclonedx/cyclonedx-npm

# Or download from https://github.com/CycloneDX/cyclonedx-cli/releases
```

### For CMake Projects (C++)

```bash
# Navigate to build directory
cd Build/CMake/build

# Generate SBOM using CMake's dependency information
cmake --build . --target sbom

# Or manually generate
cyclonedx-cli sbom \
  --input-file CMakeCache.txt \
  --output-file ../../../docs/sbom/sbom-$(cat ../../../VERSION).json \
  --format json
```

### For npm Dependencies (Frontend)

```bash
# Navigate to frontend directory
cd Tests/frontend

# Generate SBOM
cyclonedx-npm --output-file ../../docs/sbom/sbom-frontend.json

# Include in main SBOM
cyclonedx-cli merge \
  --input-files docs/sbom/sbom-main.json docs/sbom/sbom-frontend.json \
  --output-file docs/sbom/sbom-complete.json
```

### Automated SBOM Generation

SBOM generation is integrated into the release process:

1. **CI/CD Pipeline:** SBOM automatically generated during release builds
2. **Pre-Release Check:** SBOM validated before release
3. **Vulnerability Scan:** SBOM scanned for known vulnerabilities
4. **Digital Signature:** SBOM signed with release key

## Validating an SBOM

```bash
# Validate SBOM format
cyclonedx-cli validate --input-file docs/sbom/sbom-1.0.0.json

# Check for vulnerabilities
grype sbom:docs/sbom/sbom-1.0.0.json

# Or using syft
syft sbom:docs/sbom/sbom-1.0.0.json -o table
```

## Using SBOMs for Vulnerability Management

### 1. Continuous Monitoring

Set up automated vulnerability scanning:

```bash
# Daily scan with grype
grype sbom:docs/sbom/sbom-latest.json --only-fixed

# Or with trivy
trivy sbom docs/sbom/sbom-latest.json
```

### 2. License Compliance

Check license compliance:

```bash
# Using CycloneDX CLI
cyclonedx-cli analyze --input-file docs/sbom/sbom-latest.json --licenses

# Export license report
cyclonedx-cli report \
  --input-file docs/sbom/sbom-latest.json \
  --report-format html \
  --output-file license-report.html
```

### 3. Supply Chain Analysis

Analyze dependency relationships:

```bash
# Generate dependency graph
cyclonedx-cli graph --input-file docs/sbom/sbom-latest.json --output-format svg
```

## SBOM Maintenance

### Update Frequency

SBOMs are updated:

- **For each release:** New SBOM generated and included
- **For security patches:** Updated SBOM with fixed versions
- **For dependency updates:** Regenerated when dependencies change

### Historical SBOMs

All historical SBOMs are retained:

- **Location:** This directory (`docs/sbom/`)
- **Retention:** Maintained for 10 years per EU CRA requirements
- **Access:** Available in Git history and release artifacts

### SBOM Verification

Each SBOM includes:

```json
{
  "bomFormat": "CycloneDX",
  "specVersion": "1.5",
  "serialNumber": "urn:uuid:[UUID]",
  "version": 1,
  "metadata": {
    "timestamp": "[ISO8601_TIMESTAMP]",
    "tools": [...],
    "component": {
      "type": "application",
      "name": "ProLife",
      "version": "[VERSION]"
    }
  }
}
```

## Integration with Security Tools

ProLife SBOMs are compatible with:

- **Grype** - Vulnerability scanner
- **Trivy** - Comprehensive security scanner
- **OWASP Dependency-Check** - Vulnerability identification
- **Syft** - SBOM generator and analyzer
- **GitHub Dependency Graph** - Native GitHub integration
- **Dependabot** - Automated dependency updates

## EU CRA Compliance

SBOMs fulfill EU CRA requirements:

- **Article 14(4):** Machine-readable SBOM provided
- **Article 14(5):** SBOM includes all components and dependencies
- **Annex I, Part II:** Vulnerability information included
- **Transparency:** Public availability of SBOM

## SBOM Example Structure

```json
{
  "bomFormat": "CycloneDX",
  "specVersion": "1.5",
  "serialNumber": "urn:uuid:3e671687-395b-41f5-a30f-a58921a69b79",
  "version": 1,
  "metadata": {
    "timestamp": "2026-01-19T12:00:00Z",
    "component": {
      "type": "application",
      "bom-ref": "prolife@1.0.0",
      "name": "ProLife",
      "version": "1.0.0",
      "description": "Medical Imaging and Laboratory Information Management System",
      "supplier": {
        "name": "ImagingTools GmbH",
        "url": ["https://www.imagingtools.com"]
      }
    }
  },
  "components": [
    {
      "type": "library",
      "bom-ref": "qt@6.5.0",
      "name": "Qt",
      "version": "6.5.0",
      "licenses": [
        {
          "license": {
            "id": "LGPL-3.0-only"
          }
        }
      ],
      "purl": "pkg:generic/qt@6.5.0"
    }
  ],
  "dependencies": [
    {
      "ref": "prolife@1.0.0",
      "dependsOn": ["qt@6.5.0"]
    }
  ],
  "vulnerabilities": []
}
```

## Resources

- [CycloneDX Specification](https://cyclonedx.org/specification/overview/)
- [CycloneDX Tool Center](https://cyclonedx.org/tool-center/)
- [SBOM Best Practices](https://www.cisa.gov/sbom)
- [EU CRA SBOM Requirements](https://digital-strategy.ec.europa.eu/en/policies/cyber-resilience-act)

## Support

For SBOM-related questions:

- **Security Team:** security@imagingtools.com
- **Compliance Team:** compliance@imagingtools.com
- **GitHub Issues:** https://github.com/ImagingTools/ProLife/issues

---

**Last Updated:** 2026-01-19  
**Document Version:** 1.0
