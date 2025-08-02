#!/bin/bash

# Create .website directory structure
echo "Setting up .website directory structure..."
rm -rf .website
mkdir -p .website

# Copy docs directory to .website
echo "Copying docs to .website..."
cp -r docs/* .website/

# Build documentation from MDX files to .website/manual
echo "Building documentation from MDX files..."
node tools/build-docs.js

echo "Website build complete! Output in .website/"
