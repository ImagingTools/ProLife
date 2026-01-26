# ProLife

ProLife is a software solution for the information management of product lifecycle (hardware and software). It manages ordering of software and hardware products, licenses, accounting and provides important statistics of your eco system.

## Overview

ProLife integrates multiple components and dependent libraries to provide a complete solution:

- **ProLife Client**: Desktop application for users
- **ProLife Server**: Backend server infrastructure
- **ProLife Database**: Data persistence layer
- **ProLife APIs**: GraphQL and REST interfaces

## Dependencies

ProLife is built on top of several foundational repositories that must be cloned and built separately:

- **Acf**: Advanced Computing Framework
- **AcfSln**: ACF Solution configurations
- **ImtCore**: Imaging Tools Core library (private)
- **Lisa**: Lisa application components (private)
- **Puma**: Puma application components (private)
- **Agentino**: Agentino application components (private)

These dependencies must be available and their paths configured via environment variables before building ProLife.

## Quick Start

### Prerequisites

- Git 2.13+
- CMake 3.26+
- Qt 5 or Qt 6
- C++ compiler (Visual Studio 2015+, GCC 7+, or Clang 5+)

### Clone and Build

```bash
# Clone the repository
git clone https://github.com/ImagingTools/ProLife.git
cd ProLife

# Set up environment variables for dependencies
# You must have these repositories cloned and available separately
export IMTCOREDIR=/path/to/ImtCore
export PUMADIR=/path/to/Puma
export LISADIR=/path/to/Lisa
export ACFDIR=/path/to/Acf
export ACFSLNDIR=/path/to/AcfSln
export AGENTINODIR=/path/to/Agentino

# Or use setup script (will check environment)
./setup-environment.sh  # Linux/macOS
# Or on Windows
setup-environment.bat

# Build with CMake
cd Build/CMake
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

## Documentation

Comprehensive documentation is available in the following files:

- **[BUILDING.md](BUILDING.md)** - Detailed build instructions for all platforms
- **[RELEASE_GUIDE.md](RELEASE_GUIDE.md)** - Release preparation guide (English)
- **[RELEASE_GUIDE_RU.md](RELEASE_GUIDE_RU.md)** - Руководство по подготовке релиза (Russian)
- **[RELEASE_STRATEGY.md](RELEASE_STRATEGY.md)** - Release process and versioning strategy
- **[tools/README.md](tools/README.md)** - Automated release tools documentation
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - Contribution guidelines (if available)

## Project Structure

```
ProLife/
├── Build/             # Build configurations and scripts
│   ├── CMake/         # CMake build files
│   ├── VC15_64/       # Visual Studio 2015 configs
│   ├── VC16_64/       # Visual Studio 2017 configs
│   ├── VC17_64/       # Visual Studio 2019 configs
│   └── QMake/         # QMake build files
├── Config/            # Configuration files
├── Docs/              # Documentation and API docs
├── Impl/              # Implementation (executables and plugins)
│   ├── ProLifeClient/
│   ├── ProLifeServer/
│   ├── Plugins/
│   └── ...
├── Include/           # Header files and libraries
│   ├── prolifedata/
│   ├── prolifedb/
│   ├── prolifegql/
│   └── ...
├── Qml/               # QML/UI components
├── Sdl/               # Schema Definition Language files
├── Tests/             # Test suites
└── Install/           # Installation files and scripts
```

## Building

### CMake Build (Recommended)

```bash
cd Build/CMake
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### Visual Studio (Windows)

Open `Build/CMake/CMakeLists.txt` in Visual Studio with CMake support, or:

```cmd
cd Build\CMake\build
cmake -G "Visual Studio 16 2019" -A x64 ..
```

Then open the generated `.sln` file.

### QMake Build

```bash
cd Build/QMake
qmake
make
```

For detailed build instructions, see [BUILDING.md](BUILDING.md).

## Dependency Management

ProLife dependencies (Acf, AcfSln, ImtCore, Lisa, Puma, Agentino) must be cloned and built separately. Set the following environment variables to point to these repositories:

```bash
export IMTCOREDIR=/path/to/ImtCore
export PUMADIR=/path/to/Puma
export LISADIR=/path/to/Lisa
export ACFDIR=/path/to/Acf
export ACFSLNDIR=/path/to/AcfSln
export AGENTINODIR=/path/to/Agentino
```

On Windows:
```cmd
set IMTCOREDIR=C:\path\to\ImtCore
set PUMADIR=C:\path\to\Puma
set LISADIR=C:\path\to\Lisa
set ACFDIR=C:\path\to\Acf
set ACFSLNDIR=C:\path\to\AcfSln
set AGENTINODIR=C:\path\to\Agentino
```

## Development

### Branching Strategy

- **main**: Production-ready code
- **develop**: Integration branch for features
- **feature/**: Feature branches
- **bugfix/**: Bug fix branches
- **hotfix/**: Critical production fixes
- **release/**: Release preparation branches

### Making Changes

1. Create a feature branch from `develop`
2. Make your changes
3. Test thoroughly
4. Create a pull request
5. After review, merge to `develop`

For changes in dependency repositories, see [RELEASE_STRATEGY.md](RELEASE_STRATEGY.md).

## Testing

Run tests after building:

```bash
cd Build/CMake/build
ctest --config Release
```

Or run specific test executables from the `Bin/` directory.

## Releases

ProLife follows [Semantic Versioning](https://semver.org/): `MAJOR.MINOR.PATCH`

- Tagged releases: `v2.1.0`, `v2.1.1`, etc.
- Release candidates: `v2.1.0-rc1`
- Beta releases: `v2.1.0-beta1`

### 🚀 ProLife Release App (GitHub Workflow)

**NEW!** Use the automated GitHub workflow for streamlined release management:

**How to use:**
1. Go to **Actions** → **App Dispatch Release**
2. Click **Run workflow**
3. Fill in parameters:
   - Release version (e.g., `v2.1.0`)
   - Dependency versions
   - Dry run (test first, then real release)
4. Click **Run workflow**

The workflow will:
- ✅ **Generate** CHANGELOG
- ✅ **Update** version in CMakeLists.txt
- ✅ **Create** commit, tag, and GitHub Release
- ✅ **Build** and upload artifacts

See **[RELEASE_APP_GUIDE.md](RELEASE_APP_GUIDE.md)** for detailed instructions.

### Quick Release Preparation (Command Line)

For Russian speakers: См. **[RELEASE_GUIDE_RU.md](RELEASE_GUIDE_RU.md)** для подробной инструкции.

Use the automated tools for quick release preparation:

```bash
# One-command release preparation
./tools/quick-release.sh 2.1.0

# Or step-by-step validation
./tools/validate-release.sh validate    # Check current state
```

### Release Preparation Tools

Several scripts are provided to simplify release management:

**Automated Tools** (recommended):
- `tools/quick-release.sh` - One-command automated release preparation
- `tools/validate-release.sh` - Validation and automation toolkit

**Manual Tools**:
- `prepare-release.sh` (Linux/macOS) - Manual release management
- `prepare-release.bat` (Windows) - Manual release management

Key features:
- Create release branches automatically
- Manage dependency versions
- Validate release readiness
- Generate version reports

See:
- **[tools/README.md](tools/README.md)** - Detailed tool documentation
- **[RELEASE_GUIDE.md](RELEASE_GUIDE.md)** - Complete release guide (English)
- **[RELEASE_GUIDE_RU.md](RELEASE_GUIDE_RU.md)** - Полное руководство (Russian)
- **[RELEASE_STRATEGY.md](RELEASE_STRATEGY.md)** - Release strategy and process

## Contributing

We welcome contributions! Please follow these guidelines:

1. Check existing issues or create a new one
2. Fork the repository
3. Create a feature branch
4. Make your changes with clear commit messages
5. Submit a pull request

For detailed guidelines, see [CONTRIBUTING.md](CONTRIBUTING.md) (if available).

## License

Copyright (C) 2017-2020 ImagingTools GmbH

This software may be used under the terms of the ImagingTools License Agreement. See `Install/Commercial/License.txt` for details.

## Support

For questions or issues:

- GitHub Issues: https://github.com/ImagingTools/ProLife/issues
- Documentation: See `Docs/` directory
- Contact: development team

## Acknowledgments

ProLife is built on the following open-source and proprietary components:

- Qt Framework
- CMake
- And various other libraries (see individual submodules)

---

**Version**: 1.0  
**Last Updated**: 2026-01-19
