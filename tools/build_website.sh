#!/bin/bash
# Build JSAR Documentation using VitePress

echo "Building JSAR documentation with VitePress..."

# Change to project root
cd "$(dirname "$0")/.."

# Build the documentation
npm run docs:build

echo "Documentation build completed!"
echo "Output available in docs/.vitepress/dist/"
