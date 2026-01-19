# EU Declaration of Conformity

## Product Identification

**Product Name:** ProLife Medical Imaging and Laboratory Information Management System  
**Product Type:** Software with Digital Elements  
**License Type:** Commercial (Proprietary) - ImagingTools Enterprise License Agreement  
**Product Version:** [VERSION]  
**Product Code:** [PRODUCT_CODE]  
**Release Date:** [RELEASE_DATE]

## Manufacturer

**Name:** ImagingTools GmbH  
**Address:**  
[Address Line 1]  
[Address Line 2]  
[City, Postal Code]  
[Country]

**Contact:**  
Email: compliance@imagingtools.com  
Web: https://www.imagingtools.com  
Security Contact: security@imagingtools.com

## Declaration

This declaration of conformity is issued under the sole responsibility of the manufacturer.

We, ImagingTools GmbH, declare that the product described above is in conformity with:

### Applicable EU Legislation

- **Regulation (EU) 2024/2847** - Cyber Resilience Act (CRA)
  - Annex I: Essential Cybersecurity Requirements
  - Article 13: Security by Design and Default
  - Article 14: Vulnerability Handling
  - Article 15: Security Updates
  - Article 24: Technical Documentation

### Harmonized Standards Applied

The following harmonized standards have been applied:

- **ETSI EN 303 645** - Cyber Security for Consumer Internet of Things
- **ISO/IEC 27001:2013** - Information Security Management Systems
- **ISO/IEC 27034** - Application Security
- **NIST Cybersecurity Framework** - Core functions and implementation tiers

### Conformity Assessment Procedure

The conformity assessment has been performed using:

**Module A - Internal Production Control** as defined in Annex IV of the EU CRA

- Technical documentation prepared and maintained
- Manufacturing/development process ensures conformity
- Internal controls verify conformity for each release
- Documentation retained for 10 years after product placement

## Essential Cybersecurity Requirements Compliance

### 1. Security Properties (Annex I, Section 1)

✓ The product is designed and developed to ensure appropriate levels of cybersecurity

**Implemented Controls:**
- Secure software development lifecycle (SDLC)
- Regular security testing and code reviews
- Threat modeling and risk assessment
- Security-by-design principles applied throughout development
- Defense-in-depth architecture

### 2. Vulnerability Handling (Annex I, Section 2)

✓ The product is delivered without known exploitable vulnerabilities

**Implemented Controls:**
- Dependency vulnerability scanning
- SAST and DAST security testing
- Penetration testing before release
- CVE database monitoring
- Security patch management process

### 3. Secure by Default (Annex I, Section 3)

✓ The product is configured securely by default

**Implemented Controls:**
- Minimal services enabled by default
- Strong authentication required
- Secure default passwords prohibited
- Encrypted communications enabled by default
- Principle of least privilege applied

### 4. Protection Against Unauthorized Access (Annex I, Section 4)

✓ The product protects against unauthorized access

**Implemented Controls:**
- Role-based access control (RBAC)
- Multi-factor authentication support
- Session management and timeout
- Audit logging of access attempts
- Credential storage using industry-standard encryption

### 5. Confidentiality and Integrity (Annex I, Section 5)

✓ The product ensures confidentiality and integrity of data

**Implemented Controls:**
- Data encryption at rest (AES-256)
- Data encryption in transit (TLS 1.3)
- Data integrity verification (checksums, digital signatures)
- Secure key management
- Data backup and recovery procedures

### 6. Minimize Impact (Annex I, Section 6)

✓ The product is designed to minimize impact of security incidents

**Implemented Controls:**
- Error handling without information disclosure
- Graceful degradation on component failure
- Isolation of critical components
- Rate limiting and abuse prevention
- Incident response procedures

### 7. Availability (Annex I, Section 7)

✓ The product reduces impact of security incidents on availability

**Implemented Controls:**
- Redundancy and failover capabilities
- DDoS mitigation guidelines
- Resource management and quotas
- Health monitoring and alerting
- Disaster recovery procedures

### 8. Security Logging (Annex I, Section 8)

✓ The product records security-relevant events

**Implemented Controls:**
- Comprehensive audit logging
- Tamper-evident log storage
- Security event monitoring
- Log retention policies
- SIEM integration support

### 9. Secure Updates (Annex I, Section 9)

✓ The product can be securely updated

**Implemented Controls:**
- Digitally signed updates
- Secure update delivery mechanism
- Automatic update notifications
- Rollback capability
- Update verification before installation

### 10. Security Vulnerabilities (Annex I, Section 10)

✓ The manufacturer handles vulnerabilities effectively

**Implemented Controls:**
- Coordinated vulnerability disclosure program
- Security advisory process
- Timely security patch releases
- CVE assignment and tracking
- User notification of security issues

## Software Bill of Materials (SBOM)

A comprehensive Software Bill of Materials (SBOM) is provided with this product:

- **Format:** CycloneDX 1.5 (JSON)
- **Location:** `docs/sbom/sbom-[VERSION].json`
- **Components:** All libraries, dependencies, and third-party components
- **Vulnerabilities:** Known vulnerabilities documented and tracked
- **Licenses:** All component licenses identified

## Support Duration and Security Updates

**Security Update Period:** Minimum 5 years from release date  
**Support Level:** Active security updates and patches  
**End of Support Date:** [CALCULATED: RELEASE_DATE + 5 years]

Security updates will be provided for:
- Critical vulnerabilities: 14 days
- High severity: 30 days  
- Medium severity: 60 days
- Low severity: Next regular release

## Instructions for Secure Use

Detailed instructions for secure installation, configuration, and operation are provided in:

- `README.md` - General product information
- `SECURITY.md` - Security policy and vulnerability reporting
- `BUILDING.md` - Secure build instructions
- `docs/` - Comprehensive user and administrator documentation

## Technical Documentation

Complete technical documentation as required by Article 24 of the EU CRA is maintained and includes:

1. General description of the product
2. Conceptual design and manufacturing drawings
3. Risk assessment and mitigation measures
4. Test reports and conformity certificates
5. Operating instructions and security guidelines
6. Software Bill of Materials (SBOM)
7. Vulnerability handling procedures
8. Security update process

This documentation is available to market surveillance authorities upon request.

## Unique Product Identifier

**GitHub Repository:** https://github.com/ImagingTools/ProLife  
**Version Tag:** [GIT_TAG]  
**Commit SHA:** [COMMIT_SHA]  
**SBOM Reference:** sbom-[VERSION].json

## CE Marking

The CE marking is affixed to this declaration and to the product documentation.

**CE Marking Date:** [MARKING_DATE]  
**Notified Body:** Not Applicable (Module A - Internal Control)

## Validity

This declaration is valid for:

**Product Version:** [VERSION]  
**Issue Date:** [ISSUE_DATE]  
**Signed By:** [NAME], [TITLE]  
**On behalf of:** ImagingTools GmbH

---

## Signature

**Name:** [Authorized Representative Name]  
**Title:** [Title]  
**Company:** ImagingTools GmbH  
**Date:** [Signature Date]  
**Signature:** [Digital Signature or Placeholder]

---

## Instructions for Use

**When releasing a new version:**

1. Replace all `[PLACEHOLDERS]` with actual values
2. Update version-specific information
3. Generate and reference the version-specific SBOM
4. Sign the declaration digitally
5. Include this declaration in the release package
6. Archive the signed declaration for 10 years

**Required Information:**
- `[VERSION]` - Product version number (e.g., 1.0.0)
- `[PRODUCT_CODE]` - Internal product code if applicable
- `[RELEASE_DATE]` - Date of product release (YYYY-MM-DD)
- `[GIT_TAG]` - Git tag for this release (e.g., v1.0.0)
- `[COMMIT_SHA]` - Full git commit SHA
- `[ISSUE_DATE]` - Date this declaration was issued (YYYY-MM-DD)
- `[MARKING_DATE]` - Date CE marking was affixed (YYYY-MM-DD)
- `[NAME]`, `[TITLE]` - Authorized representative information

---

**Document Version:** 1.0  
**Template Last Updated:** 2026-01-19  
**Template Maintained By:** ImagingTools Compliance Team
