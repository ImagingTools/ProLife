#!/bin/bash
# SBOM Generation Script for ProLife
# This script helps generate a Software Bill of Materials (SBOM) for EU CRA compliance

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
SBOM_DIR="${PROJECT_ROOT}/docs/sbom"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "================================================"
echo "ProLife SBOM Generator"
echo "EU Cyber Resilience Act (CRA) Compliance"
echo "================================================"
echo ""

# Check for required tools
check_tool() {
    if ! command -v "$1" &> /dev/null; then
        echo -e "${RED}Error: $1 is not installed${NC}"
        echo "Please install $1 to continue"
        return 1
    else
        echo -e "${GREEN}✓ $1 found${NC}"
        return 0
    fi
}

echo "Checking for required tools..."
TOOLS_OK=true
check_tool "git" || TOOLS_OK=false

if ! check_tool "cyclonedx-cli"; then
    echo -e "${YELLOW}Warning: cyclonedx-cli not found${NC}"
    echo "Install with: npm install -g @cyclonedx/cyclonedx-cli"
    echo "Or download from: https://github.com/CycloneDX/cyclonedx-cli/releases"
fi

echo ""

# Get version information
if [ -f "${PROJECT_ROOT}/VERSION" ]; then
    VERSION=$(cat "${PROJECT_ROOT}/VERSION")
else
    # Try to get from git tag
    VERSION=$(git describe --tags --abbrev=0 2>/dev/null || echo "1.0.0")
fi

COMMIT_SHA=$(git rev-parse HEAD)
COMMIT_SHORT=$(git rev-parse --short HEAD)
BUILD_DATE=$(date -u +"%Y-%m-%dT%H:%M:%SZ")

echo "Version Information:"
echo "  Version: ${VERSION}"
echo "  Commit: ${COMMIT_SHORT}"
echo "  Date: ${BUILD_DATE}"
echo ""

# Create SBOM directory if it doesn't exist
mkdir -p "${SBOM_DIR}"

OUTPUT_FILE="${SBOM_DIR}/sbom-${VERSION}.json"

echo "Generating SBOM..."
echo "  Output: ${OUTPUT_FILE}"
echo ""

# Generate UUID (try multiple methods)
if command -v uuidgen &> /dev/null; then
    UUID=$(uuidgen)
elif [ -r /proc/sys/kernel/random/uuid ]; then
    UUID=$(cat /proc/sys/kernel/random/uuid)
else
    # Fallback: generate pseudo-UUID from timestamp and random
    TIMESTAMP=$(date +%s%N)
    RANDOM_PART=$(( RANDOM * RANDOM ))
    UUID=$(printf "%08x-%04x-%04x-%04x-%012x" \
        $((TIMESTAMP & 0xFFFFFFFF)) \
        $((TIMESTAMP >> 32 & 0xFFFF)) \
        $(( (TIMESTAMP >> 48 & 0x0FFF) | 0x4000 )) \
        $(( (RANDOM_PART & 0x3FFF) | 0x8000 )) \
        $((RANDOM_PART & 0xFFFFFFFFFFFF)))
fi

# Generate base SBOM structure
cat > "${OUTPUT_FILE}" << EOF
{
  "bomFormat": "CycloneDX",
  "specVersion": "1.5",
  "serialNumber": "urn:uuid:${UUID}",
  "version": 1,
  "metadata": {
    "timestamp": "${BUILD_DATE}",
    "tools": [
      {
        "vendor": "ImagingTools",
        "name": "ProLife SBOM Generator",
        "version": "1.0.0"
      }
    ],
    "component": {
      "type": "application",
      "bom-ref": "prolife@${VERSION}",
      "name": "ProLife",
      "version": "${VERSION}",
      "description": "ProLife Medical Imaging and Laboratory Information Management System",
      "supplier": {
        "name": "ImagingTools GmbH",
        "url": ["https://www.imagingtools.com"],
        "contact": [
          {
            "name": "Security Team",
            "email": "security@imagingtools.com"
          }
        ]
      },
      "licenses": [
        {
          "license": {
            "name": "ImagingTools Commercial License"
          }
        }
      ],
      "externalReferences": [
        {
          "type": "website",
          "url": "https://github.com/ImagingTools/ProLife"
        },
        {
          "type": "vcs",
          "url": "https://github.com/ImagingTools/ProLife.git"
        }
      ]
    },
    "properties": [
      {
        "name": "eu:cra:compliant",
        "value": "true"
      },
      {
        "name": "build:commit",
        "value": "${COMMIT_SHA}"
      },
      {
        "name": "build:date",
        "value": "${BUILD_DATE}"
      }
    ]
  },
  "components": [],
  "dependencies": [],
  "vulnerabilities": []
}
EOF

echo -e "${GREEN}✓ CycloneDX SBOM structure generated${NC}"

# Generate SPDX format SBOM
SPDX_OUTPUT_FILE="${SBOM_DIR}/sbom-${VERSION}.spdx.json"
echo ""
echo "Generating SPDX SBOM..."
echo "  Output: ${SPDX_OUTPUT_FILE}"
echo ""

cat > "${SPDX_OUTPUT_FILE}" << 'SPDX_EOF'
{
  "spdxVersion": "SPDX-2.3",
  "dataLicense": "CC0-1.0",
  "SPDXID": "SPDXRef-DOCUMENT",
  "name": "ProLife-VERSION_PLACEHOLDER",
  "documentNamespace": "https://imagingtools.com/spdx/prolife-VERSION_PLACEHOLDER",
  "creationInfo": {
    "created": "BUILD_DATE_PLACEHOLDER",
    "creators": [
      "Organization: ImagingTools GmbH",
      "Tool: ProLife SBOM Generator-1.0.0"
    ],
    "licenseListVersion": "3.21"
  },
  "packages": [
    {
      "SPDXID": "SPDXRef-Package-ProLife",
      "name": "ProLife",
      "versionInfo": "VERSION_PLACEHOLDER",
      "downloadLocation": "https://github.com/ImagingTools/ProLife",
      "filesAnalyzed": false,
      "supplier": "Organization: ImagingTools GmbH",
      "licenseConcluded": "LicenseRef-ImagingTools-Commercial",
      "licenseDeclared": "LicenseRef-ImagingTools-Commercial",
      "copyrightText": "Copyright (C) 2017-2026 ImagingTools GmbH",
      "summary": "ProLife Medical Imaging and Laboratory Information Management System",
      "homepage": "https://github.com/ImagingTools/ProLife",
      "externalRefs": [
        {
          "referenceCategory": "SECURITY",
          "referenceType": "url",
          "referenceLocator": "https://github.com/ImagingTools/ProLife/security"
        }
      ]
    }
  ],
  "relationships": [
    {
      "spdxElementId": "SPDXRef-DOCUMENT",
      "relationshipType": "DESCRIBES",
      "relatedSpdxElement": "SPDXRef-Package-ProLife"
    }
  ],
  "hasExtractedLicensingInfos": [
    {
      "licenseId": "LicenseRef-ImagingTools-Commercial",
      "name": "ImagingTools Enterprise License Agreement",
      "extractedText": "See Install/Commercial/License.txt for full license text.\n\nThis is a commercial license agreement between ImagingTools GmbH and the licensee.",
      "seeAlsos": [
        "https://github.com/ImagingTools/ProLife/blob/main/Install/Commercial/License.txt"
      ]
    }
  ]
}
SPDX_EOF

# Replace placeholders in SPDX file
sed -i "s/VERSION_PLACEHOLDER/${VERSION}/g" "${SPDX_OUTPUT_FILE}"
sed -i "s/BUILD_DATE_PLACEHOLDER/${BUILD_DATE}/g" "${SPDX_OUTPUT_FILE}"

echo -e "${GREEN}✓ SPDX SBOM structure generated${NC}"

# Check for submodules and add them
if [ -f "${PROJECT_ROOT}/.gitmodules" ]; then
    echo ""
    echo "Detecting submodules..."
    
    # Read submodules and their paths
    while IFS= read -r line; do
        if [[ $line =~ path[[:space:]]*=[[:space:]]*(.+) ]]; then
            SUBMODULE_PATH="${BASH_REMATCH[1]}"
            SUBMODULE_NAME=$(basename "${SUBMODULE_PATH}")
            
            if [ -d "${PROJECT_ROOT}/${SUBMODULE_PATH}" ]; then
                cd "${PROJECT_ROOT}/${SUBMODULE_PATH}"
                SUBMODULE_COMMIT=$(git rev-parse HEAD 2>/dev/null || echo "unknown")
                SUBMODULE_COMMIT_SHORT=$(git rev-parse --short HEAD 2>/dev/null || echo "unknown")
                echo "  Found: ${SUBMODULE_NAME} (${SUBMODULE_COMMIT_SHORT})"
                cd "${PROJECT_ROOT}"
            fi
        fi
    done < "${PROJECT_ROOT}/.gitmodules"
fi

echo ""
echo -e "${GREEN}SBOM files generated successfully!${NC}"
echo ""
echo "Generated files:"
echo "  CycloneDX: ${OUTPUT_FILE}"
echo "  SPDX:      ${SPDX_OUTPUT_FILE}"
echo ""
echo "Next steps:"
echo "  1. Review the generated SBOMs"
echo "  2. Add detailed component information for dependencies"
echo "  3. Run vulnerability scans:"
echo "     - grype sbom:${OUTPUT_FILE}"
echo "     - trivy sbom ${SPDX_OUTPUT_FILE}"
echo "  4. Validate SBOMs:"
echo "     - cyclonedx-cli validate --input-file ${OUTPUT_FILE}"
echo "     - pyspdxtools -i ${SPDX_OUTPUT_FILE}"
echo "  5. Sign SBOMs for distribution"
echo ""
echo "For more information, see: docs/sbom/README.md"
echo ""
