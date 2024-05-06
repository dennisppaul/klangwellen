#!/bin/sh

# e.g `./update-copyright.sh ./include "Copyright (c) 2023 Dennis P Paul" "Copyright (c) 2024 Dennis P Paul"`

# Directory to search
SEARCH_DIR=$1

# Text to find
OLD_TEXT=$2 # e.g "Copyright (c) 2023 Dennis P Paul"

# Text to replace with
NEW_TEXT=$3 # e.g "Copyright (c) 2024 Dennis P Paul"

# Backup extension
BACKUP_EXT=".bak"

# Find all files in the directory and subdirectories and replace text
find "$SEARCH_DIR" \( -name "*.h" -o -name "*.cpp" -o -name "*.c" \) -type f -exec sed -i"$BACKUP_EXT" "s/$OLD_TEXT/$NEW_TEXT/g" {} +

find "$SEARCH_DIR" \( -name "*.h$BACKUP_EXT" -o -name "*.cpp$BACKUP_EXT" -o -name "*.c$BACKUP_EXT" \) -type f -exec rm {} +
