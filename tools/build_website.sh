#!/bin/bash

# Build website using Nextra
echo "Building website with Nextra..."
cd website
npm run build
cd ..

echo "Website build complete! Output in .website/"
