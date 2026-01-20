# Qt LGPL-3.0 License Compliance

## Overview

ProLife uses the Qt Framework under the **GNU Lesser General Public License v3.0 (LGPL-3.0-only)**. This document explains how ProLife complies with the LGPL-3.0 requirements when using Qt in a commercial software product.

## License Information

**Component:** Qt Framework  
**Version:** 5.x or 6.x (configurable)  
**License:** LGPL-3.0-only  
**License Text:** https://www.gnu.org/licenses/lgpl-3.0.html  
**Copyright:** Copyright (C) The Qt Company Ltd.  
**Website:** https://www.qt.io

## LGPL-3.0 Compliance Requirements

The LGPL-3.0 license allows use of Qt in commercial/proprietary software under specific conditions. ProLife fully complies with all LGPL-3.0 requirements:

### 1. Dynamic Linking (LGPL-3.0 Section 4)

**Requirement:** The Combined Work (ProLife + Qt) must allow modification of the Library (Qt) and reverse engineering for debugging such modifications.

**ProLife Compliance:**
- ✅ **Dynamic Linking Only:** ProLife uses Qt exclusively as dynamically linked libraries (.dll on Windows, .so on Linux, .dylib on macOS)
- ✅ **No Static Linking:** Qt libraries are NOT statically compiled into ProLife executables
- ✅ **Runtime Library Loading:** Qt libraries are loaded at runtime and can be replaced independently
- ✅ **Interface Headers Provided:** Qt header files are publicly available from Qt distribution

**Technical Implementation:**
```
ProLife Binary Structure:
├── ProLifeClient.exe (proprietary)
├── Qt6Core.dll (LGPL, replaceable)
├── Qt6Widgets.dll (LGPL, replaceable)
├── Qt6Gui.dll (LGPL, replaceable)
└── [other Qt libraries] (LGPL, replaceable)
```

### 2. User Modification Rights (LGPL-3.0 Section 4(d)(0))

**Requirement:** Users must be able to recombine or relink the application with a modified version of the Library.

**ProLife Compliance:**
- ✅ **Replaceable Qt Libraries:** End users can replace Qt library files with modified versions
- ✅ **Standard Qt API:** ProLife uses only public Qt APIs, ensuring compatibility with modified Qt versions
- ✅ **No Qt Source Modifications:** ProLife does not modify Qt source code
- ✅ **Version Compatibility:** ProLife is tested with multiple Qt versions to ensure flexibility

**How Users Can Replace Qt:**
1. Download Qt source code from https://download.qt.io/
2. Build modified Qt libraries with same version
3. Replace Qt .dll/.so/.dylib files in ProLife installation directory
4. ProLife will use the modified Qt libraries

### 3. Source Code Availability (LGPL-3.0 Section 4(d)(1))

**Requirement:** Provide access to the Library source code.

**ProLife Compliance:**
- ✅ **Qt Source Code:** Available at https://download.qt.io/official_releases/qt/
- ✅ **Version Information:** ProLife documentation specifies exact Qt version used
- ✅ **SBOM Transparency:** Software Bill of Materials (SBOM) identifies Qt version and license
- ✅ **No Modifications:** Since ProLife doesn't modify Qt, standard Qt source applies

### 4. Installation Information (LGPL-3.0 Section 4(d))

**Requirement:** Provide Installation Information necessary to install and execute modified versions.

**ProLife Compliance:**
- ✅ **Installation Documentation:** Instructions provided for replacing Qt libraries
- ✅ **Dependency Information:** Qt dependencies and requirements documented
- ✅ **Build Information:** Qt build configuration compatible with standard Qt builds
- ✅ **No DRM/Signing:** ProLife executables do not prevent loading of modified Qt libraries

**Installation Instructions for Modified Qt:**

```bash
# Linux/macOS
cd /path/to/ProLife/installation
# Replace Qt libraries with modified versions
cp /path/to/modified/libQt6Core.so.6 ./lib/
cp /path/to/modified/libQt6Widgets.so.6 ./lib/
# Run ProLife - it will use modified Qt
./ProLifeClient

# Windows
cd C:\Program Files\ProLife
# Replace Qt DLLs with modified versions
copy C:\modified\Qt6Core.dll .
copy C:\modified\Qt6Widgets.dll .
# Run ProLife - it will use modified Qt
ProLifeClient.exe
```

### 5. License Notice (LGPL-3.0 Section 4(a))

**Requirement:** Display appropriate copyright notices and license information.

**ProLife Compliance:**
- ✅ **About Dialog:** ProLife displays Qt copyright and LGPL license in About dialog
- ✅ **Documentation:** README.md and compliance docs mention Qt usage under LGPL
- ✅ **License Files:** Qt LGPL license text included in distribution
- ✅ **SBOM:** Qt license clearly identified in Software Bill of Materials

**License Attribution in ProLife:**
```
About ProLife > Legal Information:
"This application uses Qt, copyright The Qt Company Ltd.
Qt is licensed under LGPL-3.0-only.
See licenses/Qt_LGPL-3.0.txt for details."
```

### 6. No Additional Restrictions (LGPL-3.0 Section 10)

**Requirement:** Cannot impose further restrictions beyond LGPL terms.

**ProLife Compliance:**
- ✅ **No DRM:** ProLife does not use Digital Rights Management that prevents Qt modification
- ✅ **No Anti-Debugging:** ProLife does not implement anti-debugging measures that prevent Qt debugging
- ✅ **No Code Signing Restrictions:** Modified Qt libraries can be loaded without code signing
- ✅ **No License Conflicts:** ProLife's commercial license does not restrict Qt usage rights

### 7. Patent Grant (LGPL-3.0 Section 11)

**Requirement:** Automatic patent license for covered claims.

**ProLife Compliance:**
- ✅ **No Patent Claims:** ImagingTools does not assert patent claims against Qt users
- ✅ **Qt Patent Rights:** Qt's patent grants remain intact and unaffected
- ✅ **User Rights Preserved:** End users receive full Qt patent rights under LGPL

## Qt Components Used in ProLife

ProLife uses the following Qt modules, all under LGPL-3.0-only:

| Qt Module | Purpose | Version |
|-----------|---------|---------|
| Qt Core | Core non-GUI functionality | 5.x/6.x |
| Qt Widgets | Desktop UI widgets | 5.x/6.x |
| Qt Quick/QuickWidgets | QML-based UI | 5.x/6.x |
| Qt GUI | GUI base classes | 5.x/6.x |
| Qt XML | XML processing | 5.x/6.x |
| Qt Network | Network programming | 5.x/6.x |
| Qt SVG | SVG rendering | 5.x/6.x |
| Qt SQL | Database integration | 5.x/6.x |
| Qt WebSockets | WebSocket protocol | 5.x/6.x |
| Qt QML | QML engine | 5.x/6.x |
| Qt Concurrent | Multi-threading | 5.x/6.x |
| Qt QuickControls2 | QML controls | 5.x/6.x |
| Qt Core5Compat | Qt5 compatibility (Qt6 only) | 6.x |

All modules are used via dynamic linking and can be individually replaced.

## Conveying Modified Versions (LGPL-3.0 Section 3)

If ProLife were to modify Qt source code (currently not done), we would:

1. **Mark Modifications:** Clearly identify all changes to Qt source
2. **Provide Source:** Make modified Qt source code available
3. **Use Same License:** Distribute modified Qt under LGPL-3.0
4. **Document Changes:** Maintain changelog of Qt modifications
5. **Upstream Contribution:** Consider contributing improvements to Qt Project

**Current Status:** ProLife does NOT modify Qt source code, only uses it as-is.

## Combined Work Distribution (LGPL-3.0 Section 4)

ProLife + Qt is a "Combined Work" under LGPL-3.0 terms.

**Distribution Method:**
- **ProLife Executable:** Proprietary, under ImagingTools Commercial License
- **Qt Libraries:** LGPL-3.0, distributed as separate dynamic libraries
- **Clear Separation:** ProLife code and Qt code remain separate and identifiable
- **User Rights:** Users can modify/replace Qt portion while ProLife executable remains proprietary

## Commercial Qt License Alternative

**Dual Licensing:** Qt is also available under commercial license from The Qt Company.

**Benefits of Commercial Qt License:**
- No LGPL compliance requirements
- Additional support from The Qt Company
- Proprietary Qt modifications allowed
- Enhanced commercial support

**Current Status:** ProLife uses Qt under LGPL-3.0. Customers requiring commercial Qt license can:
1. Purchase commercial Qt license separately
2. Replace LGPL Qt libraries with commercially licensed Qt
3. Continue using ProLife without LGPL obligations for Qt

## Compliance Verification

### For ProLife Users

To verify LGPL compliance:

```bash
# Check Qt library linking (Linux)
ldd /path/to/ProLifeClient | grep Qt
# Should show dynamically linked Qt libraries

# Check Qt library linking (macOS)
otool -L /path/to/ProLifeClient.app/Contents/MacOS/ProLifeClient | grep Qt

# Check Qt library linking (Windows)
dumpbin /dependents ProLifeClient.exe | findstr Qt
# Should show Qt DLL dependencies
```

### For Developers

Build configuration ensures LGPL compliance:

```cmake
# CMakeLists.txt - Dynamic Qt linking enforced
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
target_link_libraries(ProLifeClient Qt6::Core Qt6::Widgets)
# No STATIC keyword - ensures dynamic linking
```

## Documentation and Support

### Qt Documentation
- **Qt Documentation:** https://doc.qt.io/
- **LGPL FAQ:** https://www.gnu.org/licenses/gpl-faq.html#LGPLv3
- **Qt Licensing:** https://www.qt.io/licensing/

### ProLife Documentation
- **SBOM:** See `docs/sbom/` for complete Qt version and license information
- **Third-Party Components:** See `docs/compliance/THIRD_PARTY_COMPONENTS.md`
- **Build Instructions:** See `BUILDING.md` for Qt requirements

### Contact Information

For Qt LGPL compliance questions:
- **ProLife Compliance:** compliance@imagingtools.com
- **Qt Licensing Questions:** https://www.qt.io/contact-us
- **LGPL Legal Questions:** Consult with legal counsel

## Acknowledgments

ProLife acknowledges and respects the Qt Project and The Qt Company's contribution to open-source software. Qt's availability under LGPL enables ProLife to provide a high-quality cross-platform medical imaging solution.

**Attribution:**
> "This application uses Qt, a cross-platform application framework.  
> Qt is copyright The Qt Company Ltd and licensed under LGPL-3.0-only.  
> For more information, visit https://www.qt.io"

## Compliance Checklist

- [x] Qt used only via dynamic linking
- [x] No static linking of Qt libraries
- [x] No modifications to Qt source code
- [x] Qt source code location documented
- [x] Qt version information in SBOM
- [x] Installation information provided for Qt replacement
- [x] License notices displayed in application
- [x] LGPL-3.0 license text included in distribution
- [x] No additional restrictions imposed on Qt
- [x] User rights to modify Qt preserved
- [x] Patent rights preserved
- [x] Build configuration ensures dynamic linking
- [x] Documentation references Qt licensing

## Updates and Maintenance

This compliance document is reviewed and updated:
- When Qt version is upgraded
- When LGPL license interpretation changes
- Annually as part of compliance review
- When ProLife distribution method changes

**Last Reviewed:** 2026-01-20  
**Document Version:** 1.0  
**Applies to ProLife Version:** 1.0.0 and later  
**Qt Versions Covered:** Qt 5.x and Qt 6.x under LGPL-3.0-only
