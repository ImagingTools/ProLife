# TeamCity Integration Setup Guide

This document describes the TeamCity integration for the ProLife release workflow and the required configuration.

## Overview

The GitHub Actions release workflow (`release.yml`) has been refactored to offload build and artifact packaging to TeamCity. The workflow now:

1. ✅ Checks out the repository and all submodules
2. ✅ Updates submodule versions per workflow inputs
3. ✅ Generates and commits the `SUBMODULE_VERSIONS.md` report
4. ✅ Creates and pushes release tags
5. ✅ Triggers a TeamCity build via REST API
6. ❌ No longer builds artifacts locally
7. ❌ No longer packages artifacts in GitHub Actions
8. ❌ No longer uploads artifacts to GitHub Releases

## Required GitHub Secrets

The following repository secrets must be configured in GitHub Settings → Secrets and variables → Actions:

### 1. `TEAMCITY_TOKEN`
- **Description**: Bearer token for TeamCity REST API authentication
- **How to obtain**:
  1. Log in to your TeamCity instance
  2. Go to your user profile → Access Tokens
  3. Create a new token with appropriate permissions for triggering builds
  4. Copy the token value

### 2. `TEAMCITY_URL`
- **Description**: Base URL of your TeamCity instance
- **Format**: `https://teamcity.example.com` (no trailing slash)
- **Example**: `https://build.yourcompany.com`

### 3. `TEAMCITY_BUILD_TYPE_ID`
- **Description**: The build configuration ID in TeamCity for ProLife builds
- **How to find**:
  1. Open your TeamCity build configuration
  2. Look at the URL or build configuration settings
  3. The ID is typically in format: `ProjectId_BuildConfigName`
- **Example**: `ProLife_ReleaseBuild`

## TeamCity Build Configuration

Your TeamCity build configuration should:

1. Accept parameters from the GitHub Actions trigger:
   - `env.RELEASE_TAG`: The version tag being released (e.g., `v1.2.3`)
   - `env.RELEASE_BRANCH`: The branch or tag reference to build from

2. Perform all build steps:
   - Build all submodules (Acf, AcfSln, ImtCore, Lisa, Puma, Agentino)
   - Build ProLife main application
   - Package artifacts (Windows, Linux, etc.)
   - Create installers/packages

3. Publish artifacts to the appropriate location:
   - GitHub Releases (recommended)
   - Internal artifact repository
   - Other distribution channels

## Testing the Integration

### Dry Run Test
1. Go to GitHub Actions → Release Preparation workflow
2. Click "Run workflow"
3. Set all parameters and ensure `dry_run` is set to `true`
4. Verify that:
   - Submodules are updated correctly
   - SUBMODULE_VERSIONS.md is generated
   - No TeamCity build is triggered (dry run skips this)

### Full Release Test
1. Ensure all three TeamCity secrets are configured
2. Run the workflow with `dry_run` set to `false`
3. Monitor the workflow in GitHub Actions
4. Verify the TeamCity build trigger step succeeds
5. Check TeamCity for the triggered build
6. Verify the build completes and artifacts are published

## Troubleshooting

### "Failed to trigger TeamCity build"
- Verify `TEAMCITY_TOKEN` is valid and not expired
- Check `TEAMCITY_URL` is correct and accessible
- Ensure `TEAMCITY_BUILD_TYPE_ID` matches your build configuration

### TeamCity build not starting
- Check the build configuration in TeamCity is enabled
- Verify the token has permissions to trigger builds
- Review TeamCity server logs for API errors

### HTTP Authentication Errors
- Ensure the token is configured as a Bearer token in TeamCity
- Check the Authorization header format in the workflow
- Verify no proxy or firewall is blocking the request

## Maintenance

### Updating the TeamCity URL or Build Configuration
1. Update the corresponding GitHub secret
2. No workflow changes required

### Rotating the TeamCity Token
1. Generate a new token in TeamCity
2. Update the `TEAMCITY_TOKEN` secret in GitHub
3. Test with a dry run to ensure it works

## Legacy Build Steps

The previous build steps (building locally on GitHub Actions runners) have been commented out in the workflow file for reference. These include:

- Building Acf, AcfSln, ImtCore, Lisa, Puma, Agentino submodules
- Building ProLife main application
- Packaging Windows artifacts
- Uploading to GitHub Releases

These steps can be found in the workflow file under `# LEGACY STEPS` and `# LEGACY JOB` comments.

## Support

For issues with:
- **GitHub Actions workflow**: Check the workflow run logs in GitHub Actions
- **TeamCity integration**: Check both GitHub Actions logs and TeamCity server logs
- **TeamCity builds**: Check TeamCity build logs and configuration
