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

## SBOM Formats

ProLife SBOMs are provided in **two industry-standard formats** for maximum compatibility:

### 1. CycloneDX 1.5 (JSON)

- Industry-standard format supported by the EU CRA
- Machine-readable for automated processing
- Extensible and comprehensive
- Compatible with vulnerability scanning tools
- **File:** `sbom-<version>.json`

### 2. SPDX 2.3 (JSON)

- ISO/IEC 5962:2021 international standard
- Wide industry adoption and tool support
- Comprehensive license information
- Clear copyright and attribution tracking
- **File:** `sbom-<version>.spdx.json`

Both formats provide complete transparency into the software supply chain and are accepted for regulatory compliance.

## SBOM Files

Each release of ProLife includes SBOM files in both formats:

- **CycloneDX:** `sbom-<version>.json` (e.g., `sbom-1.0.0.json`)
- **SPDX:** `sbom-<version>.spdx.json` (e.g., `sbom-1.0.0.spdx.json`)
- **Location:** This directory (`docs/sbom/`)
- **Signature:** Each SBOM is signed for verification

### Current Release

- **Latest CycloneDX:** `sbom-latest.json` (symbolic link to current version)
- **Latest SPDX:** `sbom-latest.spdx.json` (symbolic link to current version)
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
- License identifier (SPDX standardized)
- License text reference
- License compliance status
- Multiple licenses (where applicable)
- Custom/proprietary license definitions

**SPDX Format Benefits:**
- Standardized SPDX license identifiers (e.g., `LGPL-3.0-only`, `Apache-2.0`)
- Support for custom licenses (e.g., `LicenseRef-ImagingTools-Commercial`)
- Clear license declarations and conclusions
- Copyright text attribution

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

Install SBOM generation tools:

```bash
# CycloneDX CLI (for CycloneDX format)
npm install -g @cyclonedx/cyclonedx-cli

# Or download binary from https://github.com/CycloneDX/cyclonedx-cli/releases
```

For npm-specific projects, you can also use:
```bash
npm install -g @cyclonedx/cyclonedx-npm
```

**SPDX Tools:**

```bash
# SPDX SBOM Generator (for SPDX format)
# Download from https://github.com/opensbom-generator/spdx-sbom-generator/releases

# Or using Go
go install github.com/opensbom-generator/spdx-sbom-generator/cmd/generator@latest

# SPDX Tools (Python-based validation and conversion)
pip install spdx-tools
```

### For CMake Projects (C++)

**CycloneDX Format:**
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

**SPDX Format:**
```bash
# Generate SPDX SBOM for CMake project
spdx-sbom-generator -p . \
  -o ../../../docs/sbom/sbom-$(cat ../../../VERSION).spdx.json

# Or manually using the ProLife script (generates both formats)
../../../scripts/generate-sbom.sh
```

### For npm Dependencies (Frontend)

```bash
# Navigate to frontend directory
cd Tests/frontend

# Generate SBOM using cyclonedx-npm
cyclonedx-npm --output-file ../../docs/sbom/sbom-frontend.json

# Merge with main SBOM using cyclonedx-cli
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

### CycloneDX Validation

```bash
# Validate CycloneDX SBOM format
cyclonedx-cli validate --input-file docs/sbom/sbom-1.0.0.json

# Check for vulnerabilities
grype sbom:docs/sbom/sbom-1.0.0.json

# Or using syft
syft sbom:docs/sbom/sbom-1.0.0.json -o table
```

### SPDX Validation

```bash
# Validate SPDX SBOM format
pyspdxtools -i docs/sbom/sbom-1.0.0.spdx.json

# Or using spdx-tools
spdx-tools validate docs/sbom/sbom-1.0.0.spdx.json

# Check SPDX license compliance
spdx-tools check-licenses docs/sbom/sbom-1.0.0.spdx.json
```

### Converting Between Formats

```bash
# Convert CycloneDX to SPDX (using compatible tools)
cyclonedx-cli convert \
  --input-file docs/sbom/sbom-1.0.0.json \
  --output-file docs/sbom/sbom-1.0.0.spdx.json \
  --output-format spdxjson

# Convert SPDX to CycloneDX (using compatible tools)
spdx-tools convert \
  --from docs/sbom/sbom-1.0.0.spdx.json \
  --to docs/sbom/sbom-1.0.0.json \
  --format cyclonedx
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

**CycloneDX:**
```bash
# Using CycloneDX CLI
cyclonedx-cli analyze --input-file docs/sbom/sbom-latest.json --licenses

# Export license report
cyclonedx-cli report \
  --input-file docs/sbom/sbom-latest.json \
  --report-format html \
  --output-file license-report.html
```

**SPDX:**
```bash
# Check SPDX license compliance
spdx-tools check-licenses docs/sbom/sbom-latest.spdx.json

# Generate license report from SPDX
spdx-tools report \
  --input docs/sbom/sbom-latest.spdx.json \
  --format html \
  --output license-report-spdx.html

# Verify SPDX license identifiers
spdx-tools validate-licenses docs/sbom/sbom-latest.spdx.json
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

ProLife SBOMs (both CycloneDX and SPDX) are compatible with:

**Vulnerability Scanners:**
- **Grype** - Vulnerability scanner (supports both formats)
- **Trivy** - Comprehensive security scanner (supports both formats)
- **OWASP Dependency-Check** - Vulnerability identification
- **Syft** - SBOM generator and analyzer

**SBOM Tools:**
- **CycloneDX CLI** - CycloneDX format tools
- **SPDX Tools** - SPDX format validation and conversion
- **NTIA SBOM Tool** - SBOM quality checking

**Integration Platforms:**
- **GitHub Dependency Graph** - Native GitHub integration
- **Dependabot** - Automated dependency updates
- **Snyk** - Security and license scanning
- **FOSSA** - License compliance platform

## EU CRA Compliance

SBOMs fulfill EU CRA requirements:

- **Article 14(4):** Machine-readable SBOM provided (both CycloneDX and SPDX)
- **Article 14(5):** SBOM includes all components and dependencies
- **Annex I, Part II:** Vulnerability information included
- **Transparency:** Public availability of SBOM
- **ISO/IEC 5962:2021:** SPDX format is an international standard

## SBOM Example Structures

### CycloneDX Format Example

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

### SPDX Format Example

```json
{
  "spdxVersion": "SPDX-2.3",
  "dataLicense": "CC0-1.0",
  "SPDXID": "SPDXRef-DOCUMENT",
  "name": "ProLife-1.0.0",
  "documentNamespace": "https://imagingtools.com/spdx/prolife-1.0.0",
  "creationInfo": {
    "created": "2026-01-19T13:00:00Z",
    "creators": [
      "Organization: ImagingTools GmbH",
      "Tool: ProLife SBOM Generator-1.0.0"
    ]
  },
  "packages": [
    {
      "SPDXID": "SPDXRef-Package-ProLife",
      "name": "ProLife",
      "versionInfo": "1.0.0",
      "supplier": "Organization: ImagingTools GmbH",
      "licenseConcluded": "LicenseRef-ImagingTools-Commercial",
      "licenseDeclared": "LicenseRef-ImagingTools-Commercial",
      "copyrightText": "Copyright (C) 2017-2026 ImagingTools GmbH"
    },
    {
      "SPDXID": "SPDXRef-Package-Qt",
      "name": "Qt Framework",
      "versionInfo": "6.5.0",
      "supplier": "Organization: The Qt Company",
      "licenseConcluded": "LGPL-3.0-only",
      "licenseDeclared": "LGPL-3.0-only",
      "copyrightText": "Copyright (C) The Qt Company Ltd."
    }
  ],
  "relationships": [
    {
      "spdxElementId": "SPDXRef-DOCUMENT",
      "relationshipType": "DESCRIBES",
      "relatedSpdxElement": "SPDXRef-Package-ProLife"
    },
    {
      "spdxElementId": "SPDXRef-Package-ProLife",
      "relationshipType": "DEPENDS_ON",
      "relatedSpdxElement": "SPDXRef-Package-Qt"
    }
  ],
  "hasExtractedLicensingInfos": [
    {
      "licenseId": "LicenseRef-ImagingTools-Commercial",
      "name": "ImagingTools Enterprise License Agreement",
      "extractedText": "See Install/Commercial/License.txt for full license text."
    }
  ]
}
```

**Key SPDX Features:**
- **Standardized License IDs:** SPDX license identifiers (e.g., `LGPL-3.0-only`, `Apache-2.0`)
- **Custom Licenses:** Support for proprietary licenses using `LicenseRef-` prefix
- **Copyright Attribution:** Clear copyright text for each package
- **Relationship Model:** Explicit dependency relationships
- **ISO Standard:** SPDX 2.3 is ISO/IEC 5962:2021

## Resources

### CycloneDX Resources
- [CycloneDX Specification](https://cyclonedx.org/specification/overview/)
- [CycloneDX Tool Center](https://cyclonedx.org/tool-center/)
- [CycloneDX GitHub](https://github.com/CycloneDX)

### SPDX Resources
- [SPDX Specification](https://spdx.github.io/spdx-spec/)
- [SPDX License List](https://spdx.org/licenses/)
- [SPDX Tools](https://github.com/spdx/tools-python)
- [SPDX GitHub](https://github.com/spdx)

### General SBOM Resources
- [SBOM Best Practices](https://www.cisa.gov/sbom)
- [EU CRA SBOM Requirements](https://digital-strategy.ec.europa.eu/en/policies/cyber-resilience-act)
- [NTIA SBOM Resources](https://www.ntia.gov/sbom)

## Support

For SBOM-related questions:

- **Security Team:** security@imagingtools.com
- **Compliance Team:** compliance@imagingtools.com
- **GitHub Issues:** https://github.com/ImagingTools/ProLife/issues

---

**Last Updated:** 2026-01-19  
**Document Version:** 1.0
