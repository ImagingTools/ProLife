#!/bin/bash

cd "$(dirname "$0")"

FILE="../../Partitura/ProLifeVoce.arp/VersionInfo.acc.xtrsvn"

# Try to unshallow the repository if needed
git fetch --prune --unshallow 2>/dev/null || true

# Try to get the default branch name
DEFAULT_BRANCH=$(git symbolic-ref refs/remotes/origin/HEAD 2>/dev/null | sed 's@^refs/remotes/origin/@@')

# Try to get revision count from default branch first
if [ -n "$DEFAULT_BRANCH" ]; then
    REV=$(git rev-list --count "origin/$DEFAULT_BRANCH" 2>/dev/null)
fi

# If that fails, try origin/master
if [ -z "$REV" ]; then
    REV=$(git rev-list --count origin/master 2>/dev/null)
fi

# If that fails, use HEAD
if [ -z "$REV" ]; then
    REV=$(git rev-list --count HEAD 2>/dev/null)
fi

# Check if we got a valid revision count
if [ -z "$REV" ]; then
    echo "Failed to compute revision count."
    exit 1
fi

# Add offset
REV_OFFSET=$((REV + 10000))

# Check if working tree is dirty
if git diff-index --quiet HEAD -- 2>/dev/null; then
    DIRTY=0
else
    DIRTY=1
fi

echo "Git revision: $REV (version: $REV_OFFSET), dirty: $DIRTY"
echo "Processing file: $FILE"

# Get output filename by removing .xtrsvn extension
OUT="${FILE%.xtrsvn}"

# Process the template file
sed -e "s/\\\$WCREV\\\$/$REV_OFFSET/g" \
    -e "s/\\\$WCMODS?1:0\\\$/$DIRTY/g" \
    "$FILE" > "$OUT"

echo "Wrote $OUT with WCREV=$REV_OFFSET and WCMODS=$DIRTY"

# --- Logic for submodules ---

echo "Checking submodules for version scripts..."

# Navigate to repository root
REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"

# List of submodules and their script paths
SUBMODULES=(
    "3rdParty/Acf:Build/Git/GenerateVersion.sh"
    "3rdParty/AcfSln:Build/Git/UpdateVersion.sh"
    "3rdParty/ImtCore:Build/Git/UpdateVersion.sh"
    "3rdParty/Lisa:Build/Git/UpdateVersion.sh"
    "3rdParty/Puma:Build/Git/UpdateVersion.sh"
    "3rdParty/Agentino:Build/Git/UpdateVersion.sh"
)

# Iterate over submodules and execute version scripts
for entry in "${SUBMODULES[@]}"; do
    IFS=':' read -r submodule_path script_path <<< "$entry"
    
    FULL_SUBMODULE_PATH="$REPO_ROOT/$submodule_path"
    FULL_SCRIPT_PATH="$FULL_SUBMODULE_PATH/$script_path"
    
    if [ -d "$FULL_SUBMODULE_PATH" ] && [ -f "$FULL_SCRIPT_PATH" ]; then
        echo "[$submodule_path] Found $script_path. Executing..."
        (cd "$FULL_SUBMODULE_PATH" && bash "$script_path")
    else
        echo "[$submodule_path] Skipping (directory or script not found)"
    fi
done

echo "UpdateVersion completed"
