# Security Policy

## EU Cyber Resilience Act (CRA) Compliance

ProLife is designed to comply with the EU Cyber Resilience Act (Regulation (EU) 2024/2847). This document outlines our security practices, vulnerability reporting procedures, and commitment to maintaining the security of our product.

## Supported Versions

We provide security updates for the following versions of ProLife:

| Version | Supported          | End of Support |
| ------- | ------------------ | -------------- |
| 1.x.x   | :white_check_mark: | TBD            |

Security updates are provided for all actively maintained versions. We recommend always using the latest stable release.

## Reporting a Vulnerability

We take the security of ProLife seriously. If you discover a security vulnerability, please report it to us responsibly.

### How to Report

**Please do NOT report security vulnerabilities through public GitHub issues.**

Instead, please report security vulnerabilities by email to:

**Email:** security@imagingtools.com

Please include the following information in your report:

- **Type of vulnerability** (e.g., buffer overflow, SQL injection, cross-site scripting)
- **Full paths of source file(s)** related to the manifestation of the vulnerability
- **Location of the affected source code** (tag/branch/commit or direct URL)
- **Step-by-step instructions to reproduce** the issue
- **Proof-of-concept or exploit code** (if possible)
- **Impact of the vulnerability**, including how an attacker might exploit it
- **Your name and contact information** (optional, for acknowledgment purposes)

### What to Expect

After you submit a vulnerability report:

1. **Acknowledgment:** We will acknowledge receipt of your vulnerability report within **3 business days**.

2. **Assessment:** Our security team will investigate and assess the vulnerability within **10 business days**.

3. **Updates:** We will keep you informed about the progress toward fixing the vulnerability:
   - Initial assessment results
   - Expected timeline for a fix
   - Release date of the patch

4. **Resolution:** Once a fix is available:
   - We will notify you when the fix is released
   - We will publicly disclose the vulnerability details after the fix is available (typically within 90 days)
   - We will credit you in the security advisory (if you wish)

5. **Timeline:**
   - **Critical vulnerabilities:** Patched within 14 days
   - **High severity vulnerabilities:** Patched within 30 days
   - **Medium severity vulnerabilities:** Patched within 60 days
   - **Low severity vulnerabilities:** Addressed in the next regular release

### Scope

The following are **in scope** for vulnerability reports:

- ProLife Client application
- ProLife Server components
- ProLife APIs (GraphQL and REST)
- ProLife Database layer
- Authentication and authorization mechanisms
- Data encryption and protection
- Third-party dependencies distributed with ProLife

The following are **out of scope**:

- Vulnerabilities in third-party services not controlled by ImagingTools
- Social engineering attacks
- Physical attacks against ProLife infrastructure
- Denial of Service (DoS) attacks
- Issues requiring physical access to end-user systems

## Security Update Distribution

Security updates for ProLife are distributed through:

1. **GitHub Releases:** All security patches are published as GitHub releases with detailed changelogs
2. **Security Advisories:** Critical vulnerabilities are published as GitHub Security Advisories
3. **Email Notifications:** Registered users receive security update notifications
4. **In-Product Updates:** ProLife Client can notify users of available security updates (if enabled)

## Security Features

ProLife implements the following security features:

### Authentication & Authorization
- Multi-factor authentication support
- Role-based access control (RBAC)
- Session management with secure token handling
- Password policy enforcement

### Data Protection
- Encryption at rest for sensitive data
- TLS/SSL encryption for data in transit
- Secure key management
- Data anonymization capabilities

### Secure Development
- Regular security code reviews
- Automated security scanning in CI/CD pipeline
- Dependency vulnerability scanning
- Static Application Security Testing (SAST)

### Audit & Monitoring
- Comprehensive audit logging
- Security event monitoring
- Anomaly detection
- Access tracking

## Software Bill of Materials (SBOM)

In compliance with the EU CRA, ProLife provides a Software Bill of Materials (SBOM) for each release. The SBOM lists all components, libraries, and dependencies included in the software.

- **Format:** CycloneDX 1.5 (JSON)
- **Location:** Included with each release package in `docs/sbom/`
- **Online Access:** Available on GitHub Releases page

To generate or view the SBOM for your installation:

```bash
# View SBOM for the current release
cat docs/sbom/sbom.json

# Validate SBOM
cyclonedx-cli validate --input-file docs/sbom/sbom.json
```

## Vulnerability Disclosure Timeline

We follow a **coordinated vulnerability disclosure** process:

1. **Day 0:** Vulnerability reported to security@imagingtools.com
2. **Day 1-3:** Acknowledgment sent to reporter
3. **Day 4-10:** Vulnerability assessment and validation
4. **Day 11-30:** Development and testing of fix
5. **Day 31-60:** Release of security patch
6. **Day 61-90:** Public disclosure of vulnerability details

In exceptional circumstances (e.g., active exploitation in the wild), we may accelerate this timeline and coordinate with relevant authorities.

## Compliance & Certifications

ProLife is designed to comply with:

- **EU Cyber Resilience Act (CRA)** - Regulation (EU) 2024/2847
- **GDPR** - General Data Protection Regulation
- **ISO 27001** - Information Security Management
- **IEC 62304** - Medical Device Software Lifecycle (where applicable)

## Security Contacts

- **General Security Inquiries:** security@imagingtools.com
- **Vulnerability Reports:** security@imagingtools.com
- **Security Update Notifications:** Subscribe via GitHub Watch
- **Emergency Security Contact:** Available to registered customers via support portal

## Attribution & Acknowledgments

We believe in recognizing security researchers who help us improve ProLife's security. With your permission, we will:

- Credit you in security advisories
- List you in our Security Researchers Hall of Fame (SECURITY_ACKNOWLEDGMENTS.md)
- Provide a CVE co-authorship opportunity (where applicable)

## Additional Resources

- [EU Cyber Resilience Act Information](https://digital-strategy.ec.europa.eu/en/policies/cyber-resilience-act)
- [OWASP Top 10](https://owasp.org/www-project-top-ten/)
- [CWE Top 25](https://cwe.mitre.org/top25/archive/2023/2023_top25_list.html)
- [GitHub Security Advisories](https://github.com/ImagingTools/ProLife/security/advisories)

## Changes to This Policy

We may update this security policy from time to time. We will notify users of any material changes by:

- Posting a notice in the repository
- Updating the CHANGELOG.md
- Sending notifications to registered users

---

**Last Updated:** 2026-01-19  
**Policy Version:** 1.0  
**EU CRA Compliance Version:** 1.0
