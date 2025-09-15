# Contributing to JSAR Runtime

Thank you for your interest in contributing to JSAR Runtime! This document provides guidelines and information to help you contribute effectively to this revolutionary browser engine library designed for The Spatial Web.

## Table of Contents

- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Building and Testing](#building-and-testing)
- [Commit Message Conventions](#commit-message-conventions)
- [Pull Request Process](#pull-request-process)
- [Code of Conduct](#code-of-conduct)
- [License](#license)

## Getting Started

### Prerequisites

Before you begin, ensure you have the following installed:

- **Node.js** v18.16.1
- **Rust** v1.86.0 or later (with nightly toolchain)
- **Platform-specific tools:**
  - macOS: Xcode Command Line Tools
  - Linux: GCC or Clang
  - Windows: Visual Studio Build Tools

### Rust Targets

Add the required Rust targets based on your development needs:

```bash
# For Android development
rustup target add aarch64-linux-android

# For macOS development
rustup target add aarch64-apple-darwin
rustup target add x86_64-apple-darwin

# For Windows development
rustup target add x86_64-pc-windows-msvc
```

### Initial Setup

1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/jsar-runtime.git
   cd jsar-runtime
   ```
3. **Add the upstream remote**:
   ```bash
   git remote add upstream https://github.com/M-CreativeLab/jsar-runtime.git
   ```
4. **Install dependencies**:
   ```bash
   npm install
   ```

## Development Workflow

### Creating a Feature Branch

1. **Sync with upstream**:
   ```bash
   git checkout main
   git pull upstream main
   git push origin main
   ```

2. **Create a feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   # or
   git checkout -b fix/issue-number-description
   ```

### Making Changes

1. **Make your changes** following the coding standards
2. **Build and test** your changes locally
3. **Commit your changes** with descriptive messages
4. **Push to your fork**:
   ```bash
   git push origin feature/your-feature-name
   ```

## Coding Standards

JSAR Runtime follows specific style guides for different languages. Please refer to our [Coding Style Guides](docs/contributing/coding_style_guides.md) for detailed information:

- **TypeScript**: [Google TypeScript Style Guide](https://google.github.io/styleguide/tsguide.html)
- **Rust**: [Rustfmt Style Guide](https://doc.rust-lang.org/nightly/style-guide/)
- **C/C++**: [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- **C#**: [Microsoft C# Style Guide](https://docs.microsoft.com/en-us/dotnet/csharp/programming-guide/inside-a-program/coding-conventions)
- **ShaderLab**: [Unity ShaderLab Style Guide](https://docs.unity3d.com/Manual/StyleGuide-Shaders.html)

### Code Formatting

**Indentation Standard**: Use 2 spaces for indentation across all source files. This is enforced by the automated formatting tools and configured in the project's formatting configuration files.

The project includes automated formatting tools:

- **TypeScript/JavaScript**: ESLint
- **Rust**: rustfmt
- **C/C++**: clang-format

Run the linter before committing:
```bash
make lint
```

## Building and Testing

### Building the Project

1. **Build JavaScript bundle**:
   ```bash
   make jsbundle
   ```

2. **Build for your target platform**:
   ```bash
   # For macOS (universal binary)
   make darwin
   
   # For Android
   make android
   
   # For Windows
   make windows
   ```

3. **Clean build** (if needed):
   ```bash
   make darwin CLEAN=yes
   ```

### Build Options

- `CLEAN=yes`: Clean the build directory before building
- `RELEASE=yes`: Build the release version
- `INSPECTOR=yes`: Enable the inspector
- `VERBOSE=yes`: Print detailed build commands

### Running Tests

```bash
# Run Rust tests
cargo test

# Run JavaScript/TypeScript tests
npm test

# Run the full test suite
make test
```

## Commit Message Conventions

We follow Angular's [Commit Message Guidelines][] to maintain a clear project history:

[Commit Message Guidelines]: https://github.com/angular/angular/blob/20.3.0/CONTRIBUTING.md#commit

### Format
```
<type>(<scope>): <description>
<BLANK LINE>
[optional body]
<BLANK LINE>
[optional footer(s)]
```

The header(`<type>(<scope>): <description>`) is mandatory and the scope of the header is optional.

Any line of the commit message cannot be longer 100 characters! This allows the message to be easier to read on GitHub as well as in various git tools.

The footer should contain a closing reference to an issue if any.

```
<type>(<scope>): <short summary>
  │       │             │
  │       │             └─⫸ Summary in present tense. Not capitalized. No period at the end.
  │       │
  │       └─⫸ Commit Scope: common|runtime|renderer|parser|graphics|input|network|xr|
  │                          client|bindings|web-content|elements|layout|dom|css|html|canvas|
  │                          animations|window|media|scripting|webxr|webgl|
  │                          changelog|website|tests|examples|
  |                          browser|android|darwin|
  │                          inspector|devtools
  │
  └─⫸ Commit Type: build|ci|docs|feat|fix|perf|refactor|test
```

### Types
- `feat`: A new feature
- `fix`: A bug fix
- `docs`: Documentation only changes
- `style`: Changes that do not affect the meaning of the code
- `refactor`: A code change that neither fixes a bug nor adds a feature
- `perf`: A code change that improves performance
- `test`: Adding missing tests or correcting existing tests
- `build`: Changes that affect the build system or external dependencies
- `ci`: Changes to CI configuration files and scripts

### Examples

The following are **good** commit message examples:

```bash
docs(changelog): update changelog to beta.5
```

```bash
feat(webxr): add hand tracking support for spatial interactions
```

```bash
fix(rendering): resolve memory leak in texture management
```

```bash
docs(contributing): add comprehensive development guidelines
```

```bash
test(dom): add unit tests for HTML element creation
```

```bash
fix(release): need to depend on latest rxjs and zone.js

The version in our package.json gets copied to the one we publish, and users need the latest of these.
```

## Pull Request Process

### Before Submitting

1. **Ensure your code builds** without errors
2. **Run tests** and verify they pass
3. **Run the linter** and fix any style issues
4. **Update documentation** if your changes affect APIs or behavior
5. **Rebase your branch** on the latest main if needed

### Submitting Your PR

1. **Create a pull request** from your feature branch to `main`
2. **Provide a clear title** following commit message conventions
3. **Write a detailed description** including:
   - What changes were made
   - Why the changes were necessary
   - Any breaking changes
   - Screenshots (for UI changes)
   - Related issue numbers (e.g., "Closes #123")

### PR Review Process

1. **Automated checks** must pass (CI/CD pipeline)
2. **Code review** by maintainers
3. **Address feedback** and update your PR as needed
4. **Final approval** and merge by maintainers

### PR Guidelines

- Keep PRs focused and atomic (one feature/fix per PR)
- Avoid large PRs when possible
- Respond to review feedback promptly
- Be open to suggestions and improvements

## Code of Conduct

We are committed to providing a welcoming and inclusive environment for all contributors. Please be respectful and professional in all interactions.

Key principles:
- Be respectful and considerate
- Welcome newcomers and help them learn
- Focus on constructive feedback
- Assume positive intent

## License

By contributing to JSAR Runtime, you agree that your contributions will be licensed under the [Apache License 2.0](LICENSE).

---

## Additional Resources

- [User Manual](docs/manual/introduction.mdx)
- [API Documentation](docs/api/)
- [Development Documentation](docs/development-zh.md)
- [Architecture Documentation](docs/internals/)
- [DeepWiki](https://deepwiki.com/m-creativelab/jsar-runtime)

## Getting Help

If you need help or have questions:
- Check existing [GitHub Issues](https://github.com/M-CreativeLab/jsar-runtime/issues)
- Create a new issue with the "question" label
- Review the documentation in the `docs/` directory

Thank you for contributing to JSAR Runtime! 🚀