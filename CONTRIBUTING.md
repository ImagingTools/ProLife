# Contributing to ProLife

Thank you for your interest in contributing to ProLife! This document provides guidelines for contributing to the project.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Submodule Development](#submodule-development)
- [Coding Standards](#coding-standards)
- [Commit Guidelines](#commit-guidelines)
- [Pull Request Process](#pull-request-process)
- [Testing](#testing)
- [Documentation](#documentation)

## Code of Conduct

- Be respectful and inclusive
- Focus on constructive feedback
- Collaborate professionally
- Follow project conventions

## Getting Started

### Prerequisites

1. Fork the repository
2. Clone your fork with submodules:
   ```bash
   git clone --recursive https://github.com/YOUR_USERNAME/ProLife.git
   cd ProLife
   ```

3. Add upstream remote:
   ```bash
   git remote add upstream https://github.com/ImagingTools/ProLife.git
   ```

4. Set up the development environment:
   ```bash
   ./setup-environment.sh  # or setup-environment.bat on Windows
   ```

### Build and Test

Before making changes, ensure you can build the project:

```bash
cd Build/CMake
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

Run tests to verify everything works:

```bash
ctest --config Release
```

## Development Workflow

### Branch Strategy

We use Git Flow for branch management:

- **main**: Production-ready code (protected)
- **develop**: Integration branch for development
- **feature/**: New features (branch from develop)
- **bugfix/**: Bug fixes (branch from develop)
- **hotfix/**: Critical production fixes (branch from main)
- **release/**: Release preparation (branch from develop)

### Creating a Feature Branch

```bash
# Update your local repository
git checkout develop
git pull upstream develop

# Create a feature branch
git checkout -b feature/my-new-feature

# Make your changes
# ...

# Commit your changes
git add .
git commit -m "feat: add my new feature"

# Push to your fork
git push origin feature/my-new-feature
```

### Keeping Your Branch Up to Date

Regularly sync with upstream:

```bash
git checkout develop
git pull upstream develop
git checkout feature/my-new-feature
git rebase develop
```

## Submodule Development

### Working on Dependencies

When you need to modify a dependency (Acf, ImtCore, Lisa, Puma, Agentino, AcfSln):

#### 1. Navigate to the Submodule

```bash
cd 3rdParty/ImtCore
```

#### 2. Create a Feature Branch in the Submodule

```bash
git checkout -b feature/my-submodule-feature
```

#### 3. Make Your Changes

```bash
# Edit files
# ...

# Commit in the submodule
git add .
git commit -m "feat: add feature in ImtCore"
```

#### 4. Push Submodule Changes

```bash
# Push to the submodule repository
git push origin feature/my-submodule-feature

# Create PR in the submodule repository
# Wait for review and merge
```

#### 5. Update ProLife to Use New Submodule Version

After the submodule PR is merged:

```bash
cd ../..  # Back to ProLife root

# Update submodule reference
cd 3rdParty/ImtCore
git checkout main  # or master
git pull
cd ../..

# Commit the submodule pointer update in ProLife
git add 3rdParty/ImtCore
git commit -m "chore(deps): update ImtCore to include new feature"
```

#### 6. Test Integration

```bash
cd Build/CMake/build
cmake --build . --config Release --clean-first
ctest --config Release
```

### Coordinated Multi-Repository Changes

For changes spanning multiple repositories:

1. **Plan**: Document all affected repositories
2. **Implement**: Create feature branches with consistent names across repos
3. **Test**: Update ProLife to reference feature branches temporarily
4. **Merge**: Merge dependency PRs first, then update ProLife

Example for testing with feature branches:

```bash
# In ProLife root
cd 3rdParty/ImtCore
git checkout feature/cross-repo-change
cd ../Puma
git checkout feature/cross-repo-change
cd ../..

# Test the integration
cd Build/CMake/build
cmake --build . --config Release
```

### Submodule Best Practices

- **Always commit submodule changes before ProLife changes**
- **Test integration after updating submodules**
- **Document submodule updates in commit messages**
- **Use tagged versions for releases**
- **Don't leave submodules in detached HEAD state**

## Coding Standards

### C++ Style

- Follow the existing code style in the repository
- Use meaningful variable and function names
- Keep functions focused and concise
- Comment complex logic
- Prefer modern C++ features (C++11/14/17)

### CMake Style

- Use lowercase for commands
- Indent with tabs (matching existing style)
- Use meaningful target names
- Document complex configurations

### Qt/QML Style

- Follow Qt naming conventions
- Use Qt containers appropriately
- Signal/slot connections should be clear and documented
- QML: Follow Qt Quick best practices

## Commit Guidelines

We follow [Conventional Commits](https://www.conventionalcommits.org/) specification:

### Commit Message Format

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

### Types

- **feat**: New feature
- **fix**: Bug fix
- **docs**: Documentation changes
- **style**: Code style changes (formatting, no logic change)
- **refactor**: Code refactoring
- **perf**: Performance improvements
- **test**: Adding or updating tests
- **build**: Build system changes
- **ci**: CI/CD changes
- **chore**: Maintenance tasks (dependencies, etc.)

### Scopes

Common scopes:
- **client**: ProLife Client changes
- **server**: ProLife Server changes
- **db**: Database-related changes
- **gql**: GraphQL-related changes
- **deps**: Dependency updates
- **build**: Build configuration

### Examples

```bash
# Feature
git commit -m "feat(client): add user authentication dialog"

# Bug fix
git commit -m "fix(server): resolve memory leak in connection handler"

# Dependency update
git commit -m "chore(deps): update ImtCore to v1.5.0"

# Breaking change
git commit -m "feat(api)!: change authentication API

BREAKING CHANGE: Authentication now requires OAuth2 tokens instead of API keys"
```

### Commit Best Practices

- Keep commits atomic (one logical change per commit)
- Write clear, descriptive messages
- Reference issues: `Fixes #123`, `Closes #456`
- Keep the first line under 72 characters
- Use imperative mood: "add feature" not "added feature"

## Pull Request Process

### Before Submitting

1. **Update from upstream**:
   ```bash
   git checkout develop
   git pull upstream develop
   git checkout feature/my-feature
   git rebase develop
   ```

2. **Test thoroughly**:
   - Build succeeds on all configurations
   - All tests pass
   - No new warnings
   - Code follows style guidelines

3. **Update documentation** if needed

4. **Update CHANGELOG.md** for significant changes

### Creating a Pull Request

1. Push your branch to your fork:
   ```bash
   git push origin feature/my-feature
   ```

2. Go to GitHub and create a Pull Request from your branch to `develop`

3. Fill out the PR template:
   - **Title**: Clear, descriptive title following commit conventions
   - **Description**: What changes are included and why
   - **Testing**: How you tested the changes
   - **Related Issues**: Link to any related issues

### PR Template

```markdown
## Description
Brief description of the changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update
- [ ] Dependency update

## Testing
How were these changes tested?

## Checklist
- [ ] Code follows project style guidelines
- [ ] Self-review completed
- [ ] Comments added for complex code
- [ ] Documentation updated
- [ ] No new warnings
- [ ] Tests added/updated
- [ ] All tests pass
- [ ] Submodule changes merged (if applicable)
- [ ] CHANGELOG.md updated (for significant changes)

## Related Issues
Fixes #123
Closes #456
```

### Review Process

1. Automated checks must pass (CI/CD)
2. At least one approving review required
3. Address review feedback promptly
4. Keep discussions focused and professional
5. Squash commits if requested

### After Approval

Your PR will be merged by a maintainer using one of:
- **Squash and merge**: For features (preferred)
- **Rebase and merge**: For clean commit history
- **Merge commit**: For release branches

## Testing

### Running Tests

```bash
cd Build/CMake/build
ctest --config Release
```

### Writing Tests

- Add tests for new features
- Add regression tests for bug fixes
- Ensure tests are deterministic
- Mock external dependencies
- Use descriptive test names

### Test Structure

```cpp
TEST(ComponentName, FeatureName) {
    // Arrange
    Setup();
    
    // Act
    auto result = PerformAction();
    
    // Assert
    EXPECT_EQ(expected, result);
}
```

## Documentation

### Code Documentation

- Use Doxygen comments for public APIs
- Document parameters, return values, and exceptions
- Explain "why" not just "what"
- Keep documentation up to date

### Project Documentation

Update relevant files when making significant changes:

- **README.md**: Project overview and quick start
- **BUILDING.md**: Build instructions
- **RELEASE_STRATEGY.md**: Release process
- **3rdParty/README.md**: Submodule management
- **CHANGELOG.md**: Version history

### Generating API Docs

```bash
cd Docs
doxygen Doxyfile
```

## Questions?

- Check existing documentation
- Search GitHub Issues
- Ask in pull request discussions
- Contact the development team

## License

By contributing, you agree that your contributions will be licensed under the same license as the project (ImagingTools License Agreement).

---

Thank you for contributing to ProLife!
