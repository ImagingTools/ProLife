# Building ProLife

This guide explains how to build ProLife from source, including all its dependencies managed as Git submodules.

## Prerequisites

### Required Software

- **Git** 2.13 or later (for submodule support)
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

### Initial Clone

Clone the repository with all submodules:

```bash
git clone --recursive https://github.com/ImagingTools/ProLife.git
cd ProLife
```

If you already cloned without `--recursive`, initialize the submodules:

```bash
git submodule update --init --recursive
```

### Authentication for Private Repositories

Some dependencies (ImtCore, Lisa, Puma, Agentino) are private. You need:

1. **SSH Key Method** (Recommended):
   ```bash
   # Ensure your SSH key is added to your GitHub account
   ssh-add ~/.ssh/id_rsa
   
   # Configure Git to use SSH URLs
   git config --global url."git@github.com:".insteadOf "https://github.com/"
   ```

2. **Personal Access Token Method**:
   ```bash
   # Configure credential helper
   git config --global credential.helper store
   
   # You'll be prompted for credentials on first access
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
- `IMTCOREDIR`: Path to ImtCore (default: uses submodule at `3rdParty/ImtCore`)
- `PUMADIR`: Path to Puma (default: uses submodule at `3rdParty/Puma`)
- `LISADIR`: Path to Lisa (default: uses submodule at `3rdParty/Lisa`)
- `PROLIFEDIR`: Path to ProLife (default: detected from environment)

#### Using Submodules (Recommended Approach)

The build system is configured to use submodules by default. You can override environment variables to point to the submodules:

```bash
export IMTCOREDIR="$(pwd)/../../3rdParty/ImtCore"
export PUMADIR="$(pwd)/../../3rdParty/Puma"
export LISADIR="$(pwd)/../../3rdParty/Lisa"
export ACFDIR="$(pwd)/../../3rdParty/Acf"
export ACFSLNDIR="$(pwd)/../../3rdParty/AcfSln"
export AGENTINODIR="$(pwd)/../../3rdParty/Agentino"

cmake ..
```

Or use CMake directly:

```bash
cmake \
  -DIMTCOREDIR=../../3rdParty/ImtCore \
  -DPUMADIR=../../3rdParty/Puma \
  -DLISADIR=../../3rdParty/Lisa \
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

### Update All Submodules

```bash
git submodule update --remote --recursive
```

### Update Specific Submodule

```bash
cd 3rdParty/ImtCore
git checkout main
git pull
cd ../..
git add 3rdParty/ImtCore
git commit -m "Update ImtCore to latest"
```

### Rebuild After Submodule Update

```bash
cd Build/CMake/build
cmake --build . --config Release --clean-first
```

## Troubleshooting

### Submodule Issues

**Problem**: Submodule directories are empty

**Solution**:
```bash
git submodule update --init --recursive
```

**Problem**: Authentication failed for private repositories

**Solution**: Configure SSH keys or Personal Access Token as described in the Authentication section.

**Problem**: Submodule in detached HEAD state

**Solution**:
```bash
cd 3rdParty/<submodule>
git checkout main  # or master
cd ../..
```

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

**Solution**: Use submodule paths or set environment variables:
```bash
export IMTCOREDIR="$(pwd)/3rdParty/ImtCore"
export PUMADIR="$(pwd)/3rdParty/Puma"
export LISADIR="$(pwd)/3rdParty/Lisa"
```

**Problem**: Build fails with "file not found" errors

**Solution**: Ensure all submodules are initialized and updated:
```bash
git submodule status
git submodule update --init --recursive
```

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
