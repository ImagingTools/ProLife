# ProLife

ProLife is a comprehensive software solution for medical imaging and laboratory information management.

## Overview

ProLife integrates multiple components and dependent libraries to provide a complete solution:

- **ProLife Client**: Desktop application for users
- **ProLife Server**: Backend server infrastructure
- **ProLife Database**: Data persistence layer
- **ProLife APIs**: GraphQL and REST interfaces

## Dependencies

ProLife is built on top of several foundational repositories managed as Git submodules:

- **Acf**: Advanced Computing Framework
- **AcfSln**: ACF Solution configurations
- **ImtCore**: Imaging Tools Core library (private)
- **Lisa**: Lisa application components (private)
- **Puma**: Puma application components (private)
- **Agentino**: Agentino application components (private)

## Quick Start

### Prerequisites

- Git 2.13+
- CMake 3.26+
- Qt 5 or Qt 6
- C++ compiler (Visual Studio 2015+, GCC 7+, or Clang 5+)

### Clone and Build

```bash
# Clone with submodules
git clone --recursive https://github.com/ImagingTools/ProLife.git
cd ProLife

# Initialize environment (Linux/macOS)
./setup-environment.sh

# Or on Windows
setup-environment.bat

# Build with CMake
cd Build/CMake
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### Alternative: Build After Clone

If you already cloned without `--recursive`:

```bash
git submodule update --init --recursive
./setup-environment.sh  # or setup-environment.bat on Windows
```

## Documentation

Comprehensive documentation is available in the following files:

- **[BUILDING.md](BUILDING.md)** - Detailed build instructions for all platforms
- **[RELEASE_GUIDE.md](RELEASE_GUIDE.md)** - Guide for using release preparation tools
- **[RELEASE_STRATEGY.md](RELEASE_STRATEGY.md)** - Release process and versioning strategy
- **[3rdParty/README.md](3rdParty/README.md)** - Submodule management guide
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - Contribution guidelines (if available)

## Project Structure

```
ProLife/
├── 3rdParty/          # Git submodules for dependencies
│   ├── Acf/
│   ├── AcfSln/
│   ├── ImtCore/
│   ├── Lisa/
│   ├── Puma/
│   └── Agentino/
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

## Submodule Management

### Initialize Submodules

```bash
git submodule update --init --recursive
```

### Update Submodules

```bash
git submodule update --remote --recursive
```

### Authentication for Private Repositories

Some dependencies are in private repositories. Configure SSH or Personal Access Token:

**SSH Method (Recommended):**
```bash
git config --global url."git@github.com:".insteadOf "https://github.com/"
ssh-add ~/.ssh/id_rsa
```

**Personal Access Token:**
```bash
git config --global credential.helper store
```

For more details, see [3rdParty/README.md](3rdParty/README.md).

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

### Release Preparation Tools

Use the provided scripts to simplify release management:

```bash
# Linux/macOS
./prepare-release.sh --help

# Windows
prepare-release.bat --help
```

Key features:
- Create release branches automatically
- Manage submodule versions
- Pin dependencies to specific commits/tags
- Generate version reports

See [RELEASE_GUIDE.md](RELEASE_GUIDE.md) for detailed instructions and [RELEASE_STRATEGY.md](RELEASE_STRATEGY.md) for the complete release process.

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
