# JSAR Runtime Development Instructions

**ALWAYS follow these instructions first and only fallback to additional search and context gathering if the information here is incomplete or found to be in error.**

JSAR Runtime is a Web browser engine library designed for the Spatial Web, supporting WebXR, WebGL, and modern web technologies. It's a multi-language project (Rust + TypeScript + C++ + CMake) targeting multiple platforms (macOS, Android, Windows).

## Working Effectively

### Initial Setup and Dependencies

**Prerequisites - Install these EXACT versions:**
- Node.js v18.16.1 or later (tested with v20.19.4)
- Rust v1.70.0 or later with nightly toolchain: `rustup toolchain install nightly-2025-01-06`
- Platform-specific tools:
  - macOS: Xcode Command Line Tools, CMake
  - Linux: GCC/Clang, CMake (note: Rust builds not supported on Linux)
  - Windows: Visual Studio Build Tools, CMake

**Required Rust targets (install based on your needs):**
```bash
rustup target add aarch64-linux-android    # For Android development
rustup target add aarch64-apple-darwin     # For macOS ARM64
rustup target add x86_64-apple-darwin      # For macOS x86_64
rustup target add x86_64-pc-windows-msvc   # For Windows development
```

### Bootstrap and Build Process

**CRITICAL BUILD TIMING - NEVER CANCEL these operations:**

1. **Install dependencies** - 2-3 minutes, set timeout to 5+ minutes:
   ```bash
   npm install
   ```
   - **Known Issue**: If npm install fails with "npmmirror" network errors, run:
     ```bash
     rm package-lock.json
     npm config set registry https://registry.npmjs.org/
     npm install
     ```

2. **Build JavaScript bundle** - CURRENTLY BROKEN due to TypeScript compilation errors:
   ```bash
   make jsbundle
   ```
   - **Status**: Build fails with 173 TypeScript errors in type definitions
   - **Do NOT use** until TypeScript issues are resolved
   - Expected time when working: 1-2 minutes

3. **Build Rust crates** - 4-5 minutes, set timeout to 10+ minutes:
   ```bash
   # For specific platforms only:
   make darwin     # macOS universal binary (aarch64 + x86_64)
   make android    # Android aarch64
   make windows    # Windows x86_64
   ```
   - **CRITICAL**: Rust builds ONLY work on supported platforms
   - **Linux is NOT supported** - will fail with "Unsupported target: x86_64-unknown-linux-gnu"
   - Build options: `CLEAN=yes`, `RELEASE=yes`, `INSPECTOR=yes`, `VERBOSE=yes`

4. **Test Rust code** (platform-dependent):
   ```bash
   cargo test    # Only works on supported platforms
   ```

### Linting and Code Quality

**Always run these before committing - both are FAST (< 30 seconds):**

1. **TypeScript/JavaScript linting** - 1-2 seconds:
   ```bash
   npm run lint
   # or
   make lint
   ```

2. **C++ formatting check** - 15-20 seconds for ~800 files:
   ```bash
   ./tools/clang-format-check.sh
   ```

3. **Fix C++ formatting** (if needed):
   ```bash
   ./tools/clang-format-fix.sh
   ```

### Documentation

**Build documentation** - 12-15 seconds:
```bash
npm run docs:build    # Build static documentation
npm run docs:dev      # Development server with live reload
npm run docs:preview  # Preview built documentation
```

## Current Project Status and Known Issues

### CRITICAL: Build System Issues
- **JavaScript Bundle Build**: BROKEN - 173 TypeScript compilation errors
- **Jest Tests**: No tests configured - `npm test` finds 0 tests
- **Platform Support**: Limited to macOS, Android, Windows only
- **Linux Development**: Can only lint, build docs, and work on TypeScript - NO Rust builds

### Working Components
✅ **npm install** - Works (with registry workaround)  
✅ **ESLint** - Fast TypeScript linting  
✅ **clang-format** - C++ code formatting  
✅ **Documentation** - VitePress build system  
✅ **Dependencies** - All npm packages install correctly  

### Broken Components
❌ **Webpack/JS Bundle** - TypeScript compilation errors  
❌ **Jest Tests** - No test configuration  
❌ **Rust builds on Linux** - Platform not supported  
❌ **Full CI pipeline** - Due to JS bundle issues  

## Validation Scenarios

**When making changes, ALWAYS test these working components:**

1. **Code Quality Validation** (Works on all platforms):
   ```bash
   npm run lint                        # Must pass
   ./tools/clang-format-check.sh      # Must pass  
   npm run docs:build                 # Must complete successfully
   ```

2. **Platform-Specific Validation** (macOS/Windows/Android only):
   ```bash
   # Only run on supported platforms:
   make jsbundle        # Currently broken - will fail
   make darwin          # macOS only
   cargo test           # Platform-dependent
   ```

## Common Development Tasks

### Adding New TypeScript Code
1. Write your TypeScript in `lib/` directory
2. Run `npm run lint` to check style
3. **DO NOT** run `make jsbundle` until TypeScript errors are fixed
4. Build documentation: `npm run docs:build`

### Modifying C++ Code  
1. Edit files in `src/` directory
2. Run `./tools/clang-format-check.sh` 
3. Fix formatting with `./tools/clang-format-fix.sh` if needed
4. Build on supported platform: `make darwin/android/windows`

### Working with Rust Code
1. Edit files in `crates/` directory  
2. **Linux users**: Cannot test Rust builds locally
3. **macOS/Windows users**: Run `cargo check` then `make darwin/windows`

## Repository Structure (Key Locations)

```
├── lib/                 # TypeScript/JavaScript source code
├── src/                 # C++ native code
├── crates/             # Rust workspace (jsbindings, runtime_apis)
├── build/              # Build configuration and scripts
├── docs/               # VitePress documentation
├── tools/              # Development scripts
├── tests/              # C++ tests (Catch2)
├── .github/workflows/  # CI configuration
├── makefile            # Main build system
├── webpack.config.cjs  # JS bundling (currently broken)
└── tsconfig.json       # TypeScript configuration
```

## CI Pipeline Requirements

**Before committing, ensure these pass:**
```bash
npm run lint                        # ESLint must pass
./tools/clang-format-check.sh      # C++ formatting must pass
npm run docs:build                 # Documentation must build
```

**Platform-specific CI (when available):**
```bash
make jsbundle                      # Currently fails
make darwin RELEASE=yes            # macOS builds
```

## NEVER CANCEL Commands

Set these minimum timeouts to avoid premature cancellation:
- `npm install`: 5+ minutes
- `make jsbundle`: 5+ minutes (when working)
- `make darwin/android/windows`: 10+ minutes  
- `cargo test`: 10+ minutes
- `./tools/clang-format-check.sh`: 2+ minutes

## Emergency Debugging

**If npm install fails:**
```bash
rm package-lock.json
npm config set registry https://registry.npmjs.org/
npm cache clean --force
npm install
```

**If TypeScript errors block development:**
- Focus on C++ and documentation work
- Use `npm run lint` for style checking
- Build docs with `npm run docs:build`
- Avoid `make jsbundle` until errors resolved

**If on unsupported platform (Linux):**
- Work on TypeScript/JavaScript in `lib/`
- Modify documentation in `docs/` 
- Use linting and formatting tools
- Cannot test Rust builds locally

## Troubleshooting

**Common Issues:**
1. **Registry timeout errors**: Use npm registry workaround above
2. **Rust build fails**: Check if your platform is supported (macOS/Windows/Android only)
3. **TypeScript errors**: Expected - focus on other development tasks
4. **No tests found**: Expected - Jest not configured
5. **Webpack bundle errors**: Known issue - JS bundle build is broken

**Always validate your changes work by running the linting and documentation builds before submitting.**