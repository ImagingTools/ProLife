# ProLife Third-Party Dependencies

This directory contains all the repositories that ProLife depends on as Git submodules.

## Dependencies

ProLife is built on top of the following repositories:

### Public Repositories
- **Acf** - Advanced Computing Framework
- **AcfSln** - ACF Solution files and configurations

### Private Repositories (Require Authentication)
- **ImtCore** - Imaging Tools Core library
- **Lisa** - Lisa application components
- **Puma** - Puma application components
- **Agentino** - Agentino application components

## Initial Setup

After cloning the ProLife repository, initialize and update all submodules:

```bash
git submodule update --init --recursive
```

**Note:** For private repositories (ImtCore, Lisa, Puma, Agentino), you will need appropriate GitHub credentials configured. If you encounter authentication errors, ensure you have:
- SSH keys configured and added to your GitHub account
- Or GitHub Personal Access Token with appropriate permissions
- Repository access granted by the ImagingTools organization

## Updating Submodules

To update all submodules to their latest commits:

```bash
git submodule update --remote --recursive
```

To update a specific submodule:

```bash
cd 3rdParty/<submodule-name>
git checkout main  # or master, depending on the default branch
git pull
cd ../..
git add 3rdParty/<submodule-name>
git commit -m "Update <submodule-name> to latest"
```

## Working with Submodules

### Checking Submodule Status

```bash
git submodule status
```

### Switching Submodule Branches

```bash
cd 3rdParty/<submodule-name>
git checkout <branch-name>
cd ../..
git add 3rdParty/<submodule-name>
git commit -m "Switch <submodule-name> to <branch-name>"
```

### Making Changes in a Submodule

1. Navigate to the submodule directory
2. Create a branch and make your changes
3. Commit and push to the submodule repository
4. Return to the ProLife repository and commit the submodule pointer update

```bash
cd 3rdParty/<submodule-name>
git checkout -b feature/my-feature
# Make changes...
git commit -am "My changes"
git push origin feature/my-feature
cd ../..
git add 3rdParty/<submodule-name>
git commit -m "Update <submodule-name> to include my feature"
```

## Build Integration

The CMake build system has been configured to use these submodules. The environment variables previously used (IMTCOREDIR, PUMADIR, etc.) can now reference these submodule paths:

- `IMTCOREDIR` → `${PROJECT_SOURCE_DIR}/3rdParty/ImtCore`
- `PUMADIR` → `${PROJECT_SOURCE_DIR}/3rdParty/Puma`
- `LISADIR` → `${PROJECT_SOURCE_DIR}/3rdParty/Lisa`
- `ACFDIR` → `${PROJECT_SOURCE_DIR}/3rdParty/Acf`
- `ACFSLNDIR` → `${PROJECT_SOURCE_DIR}/3rdParty/AcfSln`
- `AGENTINODIR` → `${PROJECT_SOURCE_DIR}/3rdParty/Agentino`

See `BUILDING.md` in the root directory for detailed build instructions.

## Troubleshooting

### Authentication Issues

If you get authentication errors when initializing private submodules:

1. **Using HTTPS:** Configure a Personal Access Token
   ```bash
   git config --global credential.helper store
   ```

2. **Using SSH:** Ensure your SSH key is added to your GitHub account
   ```bash
   ssh-add ~/.ssh/id_rsa
   ```

3. **Convert HTTPS URLs to SSH:**
   ```bash
   git config --global url."git@github.com:".insteadOf "https://github.com/"
   ```

### Detached HEAD State

If a submodule is in a detached HEAD state:

```bash
cd 3rdParty/<submodule-name>
git checkout main  # or master
cd ../..
```

### Submodule Not Initialized

If a submodule directory is empty:

```bash
git submodule update --init 3rdParty/<submodule-name>
```

## For More Information

- [Git Submodules Documentation](https://git-scm.com/book/en/v2/Git-Tools-Submodules)
- See `RELEASE_STRATEGY.md` for information about dependency versioning and releases
