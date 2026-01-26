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

# --- Logic for dependencies ---
# Note: Dependencies should be updated in their own repositories
# This script only updates ProLife version information

echo "UpdateVersion completed"
