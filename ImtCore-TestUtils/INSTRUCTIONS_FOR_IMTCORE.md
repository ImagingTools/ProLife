# Instructions for ImtCore Repository

This document describes what needs to be done in the ImtCore repository to accept the Playwright testing utilities from ProLife.

## Overview

The common Playwright testing utilities have been extracted from ProLife and are ready to be moved to ImtCore for reuse across all ImagingTools projects.

## Required Actions in ImtCore

### 1. Create Directory Structure

Create the following directory structure in ImtCore repository:

```
ImtCore/
└── Tests/
    └── playwright-utils/
```

### 2. Copy Files

Copy all files from `ProLife/ImtCore-TestUtils/playwright-utils/` to `ImtCore/Tests/playwright-utils/`:

**Files to copy:**
- `package.json` - npm package configuration
- `index.js` - Main entry point
- `utils.js` - All testing utilities
- `global-setup.js` - Global setup factory function
- `README.md` - Full documentation (Russian)
- `README_EN.md` - Brief documentation (English)

```bash
# From ProLife repository
cd /path/to/ProLife
cp ImtCore-TestUtils/playwright-utils/* /path/to/ImtCore/Tests/playwright-utils/
```

### 3. Create Branch and Commit

```bash
cd /path/to/ImtCore
git checkout -b feature/playwright-utils

git add Tests/playwright-utils
git commit -m "feat(tests): add common Playwright testing utilities

- Add reusable test utilities for Playwright tests across all projects
- Add configurable global setup factory
- Add utilities for page interaction, forms, and screenshots
- Include comprehensive Russian and English documentation

These utilities were extracted from ProLife and are now available
for reuse in all ImagingTools projects."

git push origin feature/playwright-utils
```

### 4. Create Pull Request

Create a Pull Request in ImtCore repository with the following information:

**Title:** `feat(tests): add common Playwright testing utilities`

**Description:**
```markdown
## Overview
Add common Playwright testing utilities for reuse across ImagingTools projects.

## What's Included
- Common test utilities (clicks, forms, navigation)
- Configurable global setup factory
- Screenshot utilities with masking
- Login and authentication helpers
- Comprehensive API documentation (Russian and English)

## Origin
These utilities were extracted from ProLife's `Tests/frontend` directory and 
reorganized as a reusable npm package.

## Usage
Projects can use these utilities by adding a dependency:
```json
{
  "devDependencies": {
    "@imtcore/playwright-utils": "file:path/to/ImtCore/Tests/playwright-utils"
  }
}
```

Or after publishing:
```json
{
  "devDependencies": {
    "@imtcore/playwright-utils": "^1.0.0"
  }
}
```

## Documentation
- See README.md for full Russian documentation
- See README_EN.md for English documentation

## Testing
To test locally:
```bash
cd Tests/playwright-utils
npm install @playwright/test
node -e "const utils = require('./index.js'); console.log(Object.keys(utils));"
```

## Next Steps After Merge
1. ProLife will update to use this package
2. Other ImagingTools projects can adopt these utilities
3. Future improvements can be made centrally in ImtCore
```

### 5. Post-Merge Actions

After the PR is merged in ImtCore:

#### Option A: Local File Dependency (Development)
Projects can reference the package directly:
```json
{
  "devDependencies": {
    "@imtcore/playwright-utils": "file:../../../ImtCore/Tests/playwright-utils"
  }
}
```

#### Option B: Git Dependency
```json
{
  "devDependencies": {
    "@imtcore/playwright-utils": "git+https://github.com/ImagingTools/ImtCore.git#subdirectory=Tests/playwright-utils"
  }
}
```

#### Option C: Private npm Registry (Recommended for Production)
If ImagingTools has a private npm registry:
```bash
cd ImtCore/Tests/playwright-utils
npm publish --registry https://your-private-registry.com
```

### 6. Version Management

When making updates to the utilities in ImtCore:

1. Update version in `package.json` following semantic versioning:
   - Patch (1.0.x): Bug fixes
   - Minor (1.x.0): New features, backward compatible
   - Major (x.0.0): Breaking changes

2. Update CHANGELOG.md (create if doesn't exist)

3. Tag the release:
```bash
git tag -a playwright-utils-v1.0.0 -m "Release @imtcore/playwright-utils v1.0.0"
git push origin playwright-utils-v1.0.0
```

## Benefits

1. **Code Reuse**: All ImagingTools projects can use these utilities
2. **Central Maintenance**: Updates benefit all projects
3. **Consistent Testing**: Same patterns across all projects
4. **Documentation**: Well-documented API
5. **Version Control**: Clear versioning and dependency management

## Files Overview

### package.json
Defines the npm package `@imtcore/playwright-utils` with peer dependency on `@playwright/test`.

### index.js
Main entry point that exports all utilities and the global setup factory.

### utils.js
Contains all testing utilities:
- Page interaction (clicking, scrolling)
- Form filling
- Screenshot management
- Page stability waiting
- Authentication helpers

### global-setup.js
Factory function `createGlobalSetup(options)` for creating configurable global setup functions.

### README.md (Russian)
Comprehensive documentation including:
- Installation instructions
- API reference for all functions
- Usage examples
- Configuration examples

### README_EN.md (English)
Brief English documentation with API overview.

## Support

For questions about the utilities or their usage, contact the ProLife development team or refer to the comprehensive documentation in README.md.
