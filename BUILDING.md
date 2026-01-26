# Building ProLife

This guide explains how to build ProLife from source, including setting up external dependencies.

## Prerequisites

### Required Software

- **Git** 2.13 or later
- **CMake** 3.26 or later
- **C++ Compiler**:
  - Windows: Visual Studio 2015, 2017, or 2019 (VC15, VC16, VC17)
  - Linux: GCC 7+ or Clang 5+
  - macOS: Xcode 10+ or Clang 5+
- **Qt**: Qt5 or Qt6 (with Core, Widgets, QuickWidgets, Gui, Xml, Network, Svg, Sql, WebSockets, Qml, Concurrent, QuickControls2 modules)

### Optional Tools

- **QMake**: For QMake-based builds
- **Doxygen**: For generating documentation
- **Python 3.6+**: For build scripts

## Getting the Source Code

### Clone the Repository

Clone the repository:

```bash
git clone https://github.com/ImagingTools/ProLife.git
cd ProLife
```

### Setting Up Dependencies

ProLife requires several external dependencies that must be cloned and built separately:
- **ImtCore**: Imaging Tools Core library (private)
- **Lisa**: Lisa application components (private)
- **Puma**: Puma application components (private)
- **Acf**: Advanced Computing Framework
- **AcfSln**: ACF Solution configurations
- **Agentino**: Agentino application components (private)

Clone these repositories to a location on your system and build them according to their respective documentation.

### Configuring Environment Variables

Before building ProLife, set environment variables to point to your dependency repositories:

**Linux/macOS:**
```bash
export IMTCOREDIR=/path/to/ImtCore
export PUMADIR=/path/to/Puma
export LISADIR=/path/to/Lisa
export ACFDIR=/path/to/Acf
export ACFSLNDIR=/path/to/AcfSln
export AGENTINODIR=/path/to/Agentino
export PROLIFEDIR=$(pwd)
```

**Windows (Command Prompt):**
```cmd
set IMTCOREDIR=C:\path\to\ImtCore
set PUMADIR=C:\path\to\Puma
set LISADIR=C:\path\to\Lisa
set ACFDIR=C:\path\to\Acf
set ACFSLNDIR=C:\path\to\AcfSln
set AGENTINODIR=C:\path\to\Agentino
set PROLIFEDIR=%CD%
```

**Windows (PowerShell):**
```powershell
$env:IMTCOREDIR = "C:\path\to\ImtCore"
$env:PUMADIR = "C:\path\to\Puma"
$env:LISADIR = "C:\path\to\Lisa"
$env:ACFDIR = "C:\path\to\Acf"
$env:ACFSLNDIR = "C:\path\to\AcfSln"
$env:AGENTINODIR = "C:\path\to\Agentino"
$env:PROLIFEDIR = (Get-Location).Path
```

Alternatively, you can use the provided setup scripts which will check your environment:
```bash
./setup-environment.sh  # Linux/macOS
setup-environment.bat   # Windows
```

## Build Methods

ProLife supports multiple build systems. Choose the one that fits your workflow.

### Method 1: CMake Build (Recommended)

#### Configure

```bash
cd Build/CMake
mkdir build
cd build

# Configure with default Qt version detection
cmake ..

# Or specify Qt version explicitly
cmake -DQT_VERSION_MAJOR=6 ..

# With custom build directory
cmake -DBUILDDIR=/path/to/build ..
```

#### Build

```bash
# Build all targets
cmake --build . --config Release

# Build specific target
cmake --build . --config Release --target ProLifeClient

# Parallel build (faster)
cmake --build . --config Release -j8
```

#### Configuration Options

- `QT_VERSION_MAJOR`: Qt version to use (5 or 6)
- `WEB_COMPILE`: Enable web compatibility (default: ON)
- `BUILDDIR`: Custom build output directory
- `IMTCOREDIR`: Path to ImtCore (required - must be set via environment variable or CMake argument)
- `PUMADIR`: Path to Puma (required - must be set via environment variable or CMake argument)
- `LISADIR`: Path to Lisa (required - must be set via environment variable or CMake argument)
- `ACFDIR`: Path to Acf (required - must be set via environment variable or CMake argument)
- `ACFSLNDIR`: Path to AcfSln (required - must be set via environment variable or CMake argument)
- `AGENTINODIR`: Path to Agentino (required - must be set via environment variable or CMake argument)
- `PROLIFEDIR`: Path to ProLife (default: detected from environment)

#### Specifying Dependency Paths

You can specify dependency paths via environment variables (recommended) or directly via CMake:

**Via environment variables:**
```bash
export IMTCOREDIR=/path/to/ImtCore
export PUMADIR=/path/to/Puma
export LISADIR=/path/to/Lisa
export ACFDIR=/path/to/Acf
export ACFSLNDIR=/path/to/AcfSln
export AGENTINODIR=/path/to/Agentino

cmake ..
```

**Via CMake arguments:**
```bash
cmake \
  -DIMTCOREDIR=/path/to/ImtCore \
  -DPUMADIR=/path/to/Puma \
  -DLISADIR=/path/to/Lisa \
  -DACFDIR=/path/to/Acf \
  -DACFSLNDIR=/path/to/AcfSln \
  -DAGENTINODIR=/path/to/Agentino \
  ..
```

### Method 2: Visual Studio (Windows)

#### Using CMake Integration

1. Open Visual Studio
2. File → Open → CMake...
3. Navigate to `Build/CMake/CMakeLists.txt`
4. Select the configuration (Debug/Release)
5. Build → Build All

#### Using Generated Solution Files

1. Generate Visual Studio solution:
   ```cmd
   cd Build\CMake
   mkdir build
   cd build
   cmake -G "Visual Studio 16 2019" -A x64 ..
   ```

2. Open `ProLifeAll.sln` in Visual Studio
3. Select configuration (Debug/Release)
4. Build → Build Solution (Ctrl+Shift+B)

#### Pre-configured Solutions

ProLife includes pre-configured Visual Studio directories:

- `Build/VC15_64`: Visual Studio 2015 (64-bit)
- `Build/VC16_64`: Visual Studio 2017 (64-bit)
- `Build/VC17_64`: Visual Studio 2019 (64-bit)

These may contain project files or build scripts specific to each version.

### Method 3: QMake Build

If your project includes QMake configurations:

```bash
cd Build/QMake
qmake ProLife.pro
make
```

Or on Windows:

```cmd
cd Build\QMake
qmake ProLife.pro
nmake
```

### Method 4: Command Line Scripts

#### Windows

```cmd
cd Build\VC17_64
BuildAll.bat
```

#### Linux/macOS

```bash
cd Build/CMake
./build.sh
```

## Build Targets

### Main Executables

- **ProLifeClient**: The main client application
- **ProLifeServer**: The server component
- **ProLifeServerConfigurator**: Server configuration tool

### Libraries

- **prolifedata**: Data handling library
- **prolifedb**: Database access library
- **prolifegql**: GraphQL interface library
- **prolifeqml**: QML/UI components
- **prolifestyle**: Styling components
- **prolifesdl**: SDL (Schema Definition Language) generated code

### Plugins

- **ProLifeSettingsPlugin**: Settings management plugin

### Utilities

- **ProLifeConverter**: Data conversion utility
- **ProLifeDataPck**: Data packaging tool
- **ProLifeDbPck**: Database packaging tool
- **ProLifeGqlPck**: GraphQL packaging tool

## Build Configurations

### Debug

- Includes debug symbols
- No optimizations
- Assertions enabled
- Detailed logging

```bash
cmake --build . --config Debug
```

### Release

- Optimizations enabled
- No debug symbols
- Assertions disabled
- Minimal logging

```bash
cmake --build . --config Release
```

### RelWithDebInfo

- Optimizations enabled
- Debug symbols included
- Good for profiling and debugging optimized code

```bash
cmake --build . --config RelWithDebInfo
```

## Build Output

Build artifacts are organized in the following structure:

```
ProLife/
├── Bin/
│   ├── Debug_<platform>/      # Debug executables and DLLs
│   └── Release_<platform>/    # Release executables and DLLs
├── Lib/
│   ├── Debug_<platform>/      # Debug libraries
│   └── Release_<platform>/    # Release libraries
└── AuxInclude/
    └── <platform>/
        └── GeneratedFiles/    # Generated headers and code
```

Where `<platform>` might be `Win32_VC17`, `x64_VC17`, `Linux_GCC`, etc.

## Updating Dependencies

### Update Dependencies

To update dependencies, navigate to each dependency directory and pull the latest changes:

```bash
cd /path/to/ImtCore
git pull
cd /path/to/Puma
git pull
# ... repeat for other dependencies
```

### Rebuild After Dependency Update

```bash
cd Build/CMake/build
cmake --build . --config Release --clean-first
```

## Troubleshooting

### Dependency Issues

**Problem**: Missing dependency directories

**Solution**: Ensure all dependencies are cloned and environment variables are set correctly:
```bash
echo $IMTCOREDIR  # Should point to ImtCore directory
echo $PUMADIR     # Should point to Puma directory
# etc.
```

If not set, configure them as described in the "Configuring Environment Variables" section above.

### Build Issues

**Problem**: CMake can't find Qt

**Solution**:
```bash
# Set Qt installation path
export CMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64

# Or on Windows
set CMAKE_PREFIX_PATH=C:\Qt\6.x.x\msvc2019_64
```

**Problem**: Missing IMTCOREDIR or similar environment variable

**Solution**: Set the required environment variables:
```bash
export IMTCOREDIR=/path/to/ImtCore
export PUMADIR=/path/to/Puma
export LISADIR=/path/to/Lisa
export ACFDIR=/path/to/Acf
export ACFSLNDIR=/path/to/AcfSln
export AGENTINODIR=/path/to/Agentino
```

**Problem**: Build fails with "file not found" errors

**Solution**: Ensure all dependencies are properly built and available at the paths specified by environment variables.

**Problem**: Linking errors

**Solution**: Clean build and rebuild:
```bash
cd Build/CMake/build
rm -rf *
cmake ..
cmake --build . --config Release
```

### Qt-Specific Issues

**Problem**: Conflicting Qt versions

**Solution**: Explicitly specify Qt version:
```bash
cmake -DQT_VERSION_MAJOR=6 ..
```

**Problem**: Missing Qt modules

**Solution**: Install required Qt modules using Qt Maintenance Tool or package manager.

## Clean Build

To perform a completely clean build:

```bash
# Remove build directory
cd Build/CMake
rm -rf build

# Remove generated files
rm -rf ../../AuxInclude
rm -rf ../../Bin
rm -rf ../../Lib

# Rebuild
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Building Documentation

Generate API documentation using Doxygen:

```bash
cd Docs
doxygen Doxyfile
```

Documentation will be generated in `Docs/html/`.

On Windows:

```cmd
cd Docs
GenerateDocs.bat
```

## Running Tests

If tests are enabled (uncomment in CMakeLists.txt):

```bash
cd Build/CMake/build
ctest --config Release
```

Or run specific tests:

```bash
./Bin/Release_<platform>/ProLifeTest
```

## Installation

To install ProLife system-wide:

```bash
cd Build/CMake/build
cmake --install . --config Release
```

Or with custom prefix:

```bash
cmake --install . --config Release --prefix /opt/ProLife
```

## Cross-Platform Notes

### Windows-Specific

- Use Visual Studio Command Prompt for command-line builds
- Ensure Qt binaries are in PATH
- May need to run `vcvarsall.bat` for compiler environment

### Linux-Specific

- Install development packages: `build-essential`, `cmake`, `qt6-base-dev`
- May need to install additional Qt modules separately
- Ensure proper compiler (GCC/Clang) is installed

### macOS-Specific

- Install Xcode Command Line Tools
- Qt can be installed via Homebrew: `brew install qt`
- May need to set `CMAKE_PREFIX_PATH` to Qt installation

## Additional Resources

- `RELEASE_STRATEGY.md` - Release process and versioning
- `3rdParty/README.md` - Submodule management details
- `CONTRIBUTING.md` - Development guidelines
- CMake documentation: https://cmake.org/documentation/

## Support

For build issues or questions:

1. Check this documentation
2. Review GitHub Issues
3. Contact the development team

---

**Document Version**: 1.0  
**Last Updated**: 2026-01-19
