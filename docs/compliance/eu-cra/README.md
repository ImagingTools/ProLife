# EU Cyber Resilience Act (CRA) Compliance

## Overview

This directory contains documentation demonstrating ProLife's compliance with the EU Cyber Resilience Act (Regulation (EU) 2024/2847), which establishes mandatory cybersecurity requirements for products with digital elements placed on the EU market.

## Compliance Status

**Product:** ProLife Medical Imaging and Laboratory Information Management System  
**Manufacturer:** ImagingTools GmbH  
**Compliance Version:** 1.0  
**Last Updated:** 2026-01-19

ProLife is compliant with the essential cybersecurity requirements outlined in Annex I of the EU CRA.

## Documentation Structure

This compliance package includes:

1. **CE_Declaration_Template.md** - CE marking declaration of conformity template
2. **Technical_Documentation.md** - Technical documentation as required by Article 24
3. **Risk_Assessment.md** - Cybersecurity risk assessment documentation
4. **Vulnerability_Management.md** - Vulnerability handling and disclosure procedures
5. **SBOM.md** - Software Bill of Materials generation and maintenance

## Essential Cybersecurity Requirements

ProLife addresses the following essential requirements from Annex I of the EU CRA:

### 1. Security by Design and by Default (Article 13)

- **Secure Development Lifecycle:** ProLife follows secure coding practices and regular security reviews
- **Minimal Attack Surface:** Only necessary features and services are enabled by default
- **Defense in Depth:** Multiple layers of security controls are implemented
- **Security Testing:** Regular vulnerability assessments and penetration testing

### 2. Vulnerability Handling (Article 14)

- **Vulnerability Disclosure Policy:** Documented in SECURITY.md
- **Patch Management:** Security updates released within defined timelines
- **SBOM Maintenance:** Software Bill of Materials updated with each release
- **CVE Tracking:** Vulnerabilities tracked and assigned CVE identifiers when applicable

### 3. Security Updates (Article 15)

- **Automatic Updates:** ProLife Client supports automatic security update notifications
- **Update Delivery:** Security patches distributed through GitHub releases and customer portal
- **Update Period:** Security support provided for all active versions (see SECURITY.md)
- **Update Documentation:** Detailed changelog and security advisories with each update

### 4. Reporting Obligations (Article 16)

- **Incident Reporting:** Process for reporting actively exploited vulnerabilities to ENISA
- **Timeline Compliance:** Critical vulnerabilities reported within 24 hours of awareness
- **Notification to Users:** Users notified of security incidents affecting their installations

### 5. Identification and Documentation (Article 24)

- **Product Identification:** Unique identifiers for each release (version numbers, git tags)
- **CE Marking:** CE marking affixed to product documentation and packaging
- **Contact Information:** Manufacturer contact details for security and support inquiries
- **Instructions for Use:** Comprehensive user documentation including security guidance

## Conformity Assessment

ProLife follows **Module A (Internal Production Control)** for conformity assessment:

1. **Technical Documentation:** Maintained and available for regulatory review
2. **Manufacturing Process:** Documented development and release procedures
3. **CE Declaration:** Declaration of conformity issued for each release
4. **Documentation Retention:** All conformity documentation retained for 10 years

## Product Classification

Under the EU CRA, ProLife is classified as:

- **Category:** Class I product (based on risk assessment)
- **Criticality:** Important product with digital elements
- **Scope:** Software product for medical imaging and laboratory management
- **Intended Use:** Professional healthcare environment

## Security Features

### Authentication & Access Control
- Multi-factor authentication
- Role-based access control (RBAC)
- Session management
- Password policy enforcement

### Data Protection
- Encryption at rest (AES-256)
- Encryption in transit (TLS 1.3)
- Secure key management
- Data backup and recovery

### Audit & Monitoring
- Comprehensive audit logs
- Security event monitoring
- User activity tracking
- Anomaly detection

### Network Security
- Firewall configuration guidelines
- Network segmentation support
- Secure communication protocols
- VPN compatibility

## Vulnerability Management Process

1. **Detection:**
   - Automated dependency scanning
   - Regular security audits
   - Penetration testing
   - Responsible disclosure program

2. **Assessment:**
   - CVSS scoring
   - Impact analysis
   - Exploitability assessment
   - Risk prioritization

3. **Remediation:**
   - Patch development
   - Security testing
   - Release preparation
   - Distribution to customers

4. **Disclosure:**
   - Security advisory publication
   - CVE assignment
   - Coordinated disclosure
   - Customer notification

## Software Bill of Materials (SBOM)

ProLife provides a comprehensive SBOM for transparency and supply chain security:

- **Format:** CycloneDX 1.5 JSON
- **Content:** All components, libraries, and dependencies
- **Updates:** Generated for each release
- **Access:** Included in release packages and available online
- **Verification:** Digitally signed for authenticity

See `docs/sbom/` directory for SBOM files.

## Security Support Lifecycle

- **Active Support:** All current major versions receive security updates
- **Extended Support:** Available for enterprise customers
- **End of Life:** Announced 12 months in advance
- **Critical Updates:** Provided even after EOL for critical vulnerabilities

## Compliance Monitoring

We continuously monitor compliance through:

- **Regular Reviews:** Quarterly compliance assessments
- **Regulatory Updates:** Monitoring of EU CRA guidance and updates
- **Industry Standards:** Alignment with ETSI EN 303 645 and ISO 27001
- **Third-Party Audits:** Annual security audits by certified auditors

## Contact Information

**Manufacturer:**  
ImagingTools GmbH  
[Address Line 1]  
[Address Line 2]  
[City, Postal Code]  
[Country]

**Security Contact:**  
Email: security@imagingtools.com  
Web: https://github.com/ImagingTools/ProLife/security

**Regulatory Contact:**  
Email: compliance@imagingtools.com

**Technical Support:**  
Email: support@imagingtools.com  
Web: https://github.com/ImagingTools/ProLife/issues

## References

- [EU Cyber Resilience Act - Full Text](https://eur-lex.europa.eu/legal-content/EN/TXT/?uri=CELEX:32024R2847)
- [ENISA - European Union Agency for Cybersecurity](https://www.enisa.europa.eu/)
- [CycloneDX SBOM Standard](https://cyclonedx.org/)
- [Common Vulnerability Scoring System (CVSS)](https://www.first.org/cvss/)
- [ETSI EN 303 645 - Cyber Security for Consumer IoT](https://www.etsi.org/deliver/etsi_en/303600_303699/303645/)

## Document Control

| Version | Date       | Author           | Changes                    |
|---------|------------|------------------|----------------------------|
| 1.0     | 2026-01-19 | ImagingTools     | Initial CRA compliance doc |

---

**Note:** This documentation is maintained as part of the technical file required under Article 24 of the EU CRA. It must be kept up to date and made available to market surveillance authorities upon request.
