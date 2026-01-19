# EU CRA Compliance Quick Start Guide

This guide helps you quickly understand and implement EU Cyber Resilience Act (CRA) compliance for ProLife deployments and distributions.

**Important:** ProLife is a **commercial software product** distributed under the ImagingTools Enterprise License Agreement. This guide covers EU CRA compliance requirements for commercial software distribution.

## For Users and Administrators

### What You Need to Know

ProLife complies with the EU Cyber Resilience Act. This means:

1. **Security is Built-in:** The product is designed with security as a priority
2. **Transparency:** You can see all software components via the SBOM
3. **Timely Updates:** Security issues are fixed promptly
4. **Vulnerability Reporting:** There's a clear process to report security issues

### Key Files to Review

- **[SECURITY.md](../../SECURITY.md)** - How to report vulnerabilities
- **docs/sbom/** - Software Bill of Materials
- **docs/compliance/eu-cra/** - Complete compliance documentation

### Security Best Practices

1. **Keep Updated:** Install security updates promptly
2. **Monitor Advisories:** Watch GitHub Security Advisories
3. **Report Issues:** Use security@imagingtools.com for security concerns
4. **Follow Documentation:** Review security guidelines in the docs

## For Developers

### Before Release Checklist

- [ ] Run security scans (SAST/DAST)
- [ ] Generate SBOM: `./scripts/generate-sbom.sh`
- [ ] Scan for vulnerabilities: `grype sbom:docs/sbom/sbom-<version>.json`
- [ ] Update CHANGELOG.md with security fixes
- [ ] Review and sign CE Declaration
- [ ] Test security features
- [ ] Update security documentation

### Security Development Guidelines

1. **Secure Coding:** Follow OWASP guidelines
2. **Dependency Management:** Keep dependencies updated
3. **Code Review:** Security-focused code reviews
4. **Testing:** Include security tests
5. **Documentation:** Document security features

### Required Tools

```bash
# Install CycloneDX for SBOM generation
npm install -g @cyclonedx/cyclonedx-cli

# Install vulnerability scanner
brew install grype  # macOS
# or download from https://github.com/anchore/grype

# Install SBOM analyzer
brew install syft  # macOS
```

## For Product Managers/Release Managers

### Release Process Integration

1. **Pre-Release (T-2 weeks)**
   - Generate SBOM
   - Run vulnerability scans
   - Review security test results
   - Prepare security advisory (if needed)

2. **Release Day (T-0)**
   - Sign CE Declaration
   - Include SBOM in release package
   - Publish security advisory (if applicable)
   - Update compliance documentation

3. **Post-Release (T+1 week)**
   - Monitor for security issues
   - Track update adoption
   - Respond to security questions

### Documentation Updates

For each release, update:
- [ ] CE Declaration (with actual values)
- [ ] SBOM (generated and validated)
- [ ] CHANGELOG.md (security fixes noted)
- [ ] Version numbers in all documents
- [ ] Compliance documentation dates

## For Security Teams

### Vulnerability Handling Process

1. **Receipt:** Acknowledge within 3 business days
2. **Assessment:** Analyze within 10 days
3. **Fix Development:** Based on severity (14-60 days)
4. **Disclosure:** Coordinated with fix release
5. **Notification:** ENISA for critical issues

### Required Notifications

**To ENISA (within 24 hours):**
- Actively exploited vulnerabilities
- Severe security incidents
- Large-scale security issues

**To Users:**
- Security updates available
- Critical vulnerabilities
- Recommended actions

### Security Tools Setup

```bash
# Daily vulnerability monitoring
grype sbom:docs/sbom/sbom-latest.json --only-fixed

# Dependency scanning
npm audit  # for npm dependencies
pip-audit  # for Python dependencies

# SAST scanning
# Integrate with CI/CD pipeline
```

## For Compliance Officers

### Document Control

**Must Maintain for 10 Years:**
- CE Declarations (signed)
- SBOMs (all versions)
- Security audit reports
- Vulnerability handling records
- Customer notifications

**Regular Reviews (Quarterly):**
- [ ] Compliance status
- [ ] Security metrics
- [ ] Vulnerability handling performance
- [ ] Documentation updates
- [ ] Regulatory changes

### Audit Readiness

Keep readily accessible:
1. **Technical Documentation**
   - Product specifications
   - Architecture documentation
   - Security design documents

2. **Compliance Records**
   - CE Declarations
   - SBOM history
   - Security test reports
   - Vulnerability records

3. **Process Documentation**
   - Secure development lifecycle
   - Vulnerability management process
   - Incident response procedures
   - Change management records

## Quick Reference: Key Requirements

| Requirement | Implementation | Verification |
|------------|----------------|--------------|
| Security by Design | Secure SDLC | Code reviews, security testing |
| Vulnerability Handling | Process documented | SECURITY.md, response times tracked |
| SBOM | CycloneDX format | docs/sbom/ directory |
| Security Updates | Via GitHub releases | Update notifications, changelogs |
| CE Marking | Declaration template | docs/compliance/eu-cra/ |
| Incident Reporting | ENISA notification | Process in Vulnerability_Management.md |
| Documentation | Technical file | docs/compliance/ directory |

## Common Questions

### Q: When should SBOM be generated?
**A:** For every release, including security patches. Use `./scripts/generate-sbom.sh`

### Q: How quickly must vulnerabilities be fixed?
**A:** Depends on severity:
- Critical: 14 days
- High: 30 days
- Medium: 60 days
- Low: Next regular release

### Q: Who can report vulnerabilities?
**A:** Anyone, via security@imagingtools.com. Please do NOT use public GitHub issues.

### Q: What about third-party vulnerabilities?
**A:** Monitor dependencies, update promptly, and include in SBOM with vulnerability status.

### Q: How long must documentation be retained?
**A:** 10 years from the date of placing the product on the EU market.

### Q: What if a vulnerability is actively exploited?
**A:** Notify ENISA within 24 hours, expedite fix, notify all users immediately.

## Resources

### Internal
- [SECURITY.md](../../SECURITY.md) - Security policy
- [docs/compliance/eu-cra/](.) - Full compliance documentation
- [docs/sbom/](../sbom/) - SBOM documentation

### External
- [EU CRA Official Text](https://eur-lex.europa.eu/legal-content/EN/TXT/?uri=CELEX:32024R2847)
- [ENISA Website](https://www.enisa.europa.eu/)
- [CycloneDX](https://cyclonedx.org/)
- [CVSS Calculator](https://www.first.org/cvss/calculator/3.1)

## Support

- **General Questions:** compliance@imagingtools.com
- **Security Issues:** security@imagingtools.com
- **Technical Support:** support@imagingtools.com
- **GitHub Issues:** https://github.com/ImagingTools/ProLife/issues (non-security only)

---

**Document Version:** 1.0  
**Last Updated:** 2026-01-19  
**For:** ProLife EU CRA Compliance
