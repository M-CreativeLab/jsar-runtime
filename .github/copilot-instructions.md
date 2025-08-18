# JSAR Runtime Development Instructions

**ALWAYS follow these instructions first and only fallback to additional search and context gathering if the information here is incomplete or found to be in error.**

JSAR Runtime is a Web browser engine library designed for the Spatial Web, supporting WebXR, WebGL, and modern web technologies. It's a multi-language project with each language serving specific roles:

- **C/C++**: Core rendering server implementation, web process management, low-level graphics, performance-critical rendering operations, and Web APIs implementation
- **Rust**: Integration with [Servo][] components like [stylo][] and [cssparser][] for CSS parsing and styling
- **TypeScript**: High-level application code

Development is supported on **macOS only**, with deployment targets for macOS and Android.

[Servo]: https://github.com/servo/servo
[stylo]: https://crates.io/crates/stylo
[cssparser]: https://crates.io/crates/cssparser

## Working Effectively

### Initial Setup and Dependencies

**Prerequisites - Install these EXACT versions:**
- Node.js v18.16.1 or later (tested with v20.19.4)
- Rust v1.70.0 or later with nightly toolchain: `rustup toolchain install nightly-2025-01-06`
- macOS: Xcode Command Line Tools, CMake

**Required Rust targets:**
```bash
rustup target add aarch64-linux-android    # For Android deployment
rustup target add aarch64-apple-darwin     # For macOS ARM64
rustup target add x86_64-apple-darwin      # For macOS x86_64
```

### Bootstrap and Build Process

**CRITICAL BUILD TIMING - NEVER CANCEL these operations:**

1. **Install dependencies** - 2-3 minutes, set timeout to 5+ minutes:
   ```bash
   npm ci
   ```
   - **Note**: Use `npm ci` for clean installs, not `npm install`, but if your changes require a fresh install such as:
      -  add new npm dependencies
      -  update existing npm dependencies
      -  remove npm dependencies
   - **Known Issue**: If npm install fails with "npmmirror" network errors, run:
     ```bash
     rm package-lock.json
     npm config set registry https://registry.npmjs.org/
     npm ci
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
   ```
   - **CRITICAL**: Rust builds ONLY work on macOS for development
   - Build options: `CLEAN=yes`, `RELEASE=yes`, `INSPECTOR=yes`, `VERBOSE=yes`

4. **Test Rust code** (macOS only):
   ```bash
   cargo test    # Only works on macOS
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
- **Platform Support**: macOS development only, Android deployment supported
- **Deployment Targets**: macOS and Android only

### Working Components
✅ **npm install** - Works (with registry workaround)  
✅ **ESLint** - Fast TypeScript linting  
✅ **clang-format** - C++ code formatting  
✅ **Documentation** - VitePress build system  
✅ **Dependencies** - All npm packages install correctly  

### Broken Components
❌ **Webpack/JS Bundle** - TypeScript compilation errors  
❌ **Jest Tests** - No test configuration  
❌ **Rust builds on non-macOS** - Platform not supported  
❌ **Full CI pipeline** - Due to JS bundle issues  

## Validation Scenarios

**When making changes, ALWAYS test these working components:**

1. **Code Quality Validation** (Works on macOS):
   ```bash
   npm run lint                        # Must pass
   ./tools/clang-format-check.sh      # Must pass  
   npm run docs:build                 # Must complete successfully
   ```

2. **Platform-Specific Validation** (macOS only):
   ```bash
   # Only run on macOS:
   make jsbundle        # Currently broken - will fail
   make darwin          # macOS only
   cargo test           # macOS only
   ```

## Common Development Tasks

### Adding New TypeScript Code
1. Write your TypeScript in `lib/` directory
2. Run `npm run lint` to check style
3. **DO NOT** run `make jsbundle` until TypeScript errors are fixed
4. Build documentation: `npm run docs:build`

### Modifying C/C++ Code  
1. Edit files in `src/` directory
2. Run `./tools/clang-format-check.sh` 
3. Fix formatting with `./tools/clang-format-fix.sh` if needed
4. Build on supported platform: `make darwin/android/windows`

### Working with Rust Code
1. Edit files in `crates/` directory  
2. **macOS only**: Run `cargo check` then `make darwin`
3. Build for Android deployment: `make android`

## Repository Structure (Key Locations)

```
├── lib/                 # TypeScript/JavaScript source code
├── src/                 # C++ native code
├── crates/             # Rust workspace (jsbindings, runtime_apis)
├── build/              # Build configuration and scripts
├── docs/               # VitePress documentation
├── fixtures/           # Test fixtures
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
- `make darwin/android`: 10+ minutes  
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

**If on unsupported platform (non-macOS):**
- Development and builds are not supported
- Project requires macOS for development

## Troubleshooting

**Common Issues:**
1. **Registry timeout errors**: Use npm registry workaround above
2. **Rust build fails**: Ensure you're on macOS for development
3. **TypeScript errors**: Expected - focus on other development tasks
4. **No tests found**: Expected - Jest not configured
5. **Webpack bundle errors**: Known issue - JS bundle build is broken

**Always validate your changes work by running the linting and documentation builds before submitting.**
