# Publishing @yodaos-jsar/web-apis

This document explains how to use the `publish:webapis` tool to publish the `@yodaos-jsar/web-apis` package based on the `./types/transmute-webapis.d.ts` file.

## Purpose

The `publish:webapis` tool is used to publish the `@yodaos-jsar/web-apis` package, which contains the TypeScript definitions for the Web APIs used in the YodaOS JSAR runtime. The tool reads the version from the root `package.json` file and appends the commit short hash to the base version, or uses the base version if the `--release` flag is specified.

## Usage

To use the `publish:webapis` tool, run the following command:

```sh
npm run publish:webapis
```

This will publish the package with the version from the `package.json` file and the commit short hash appended to it.

### Releasing a new version

To release a new version of the package, use the `--release` flag:

```sh
npm run publish:webapis -- --release
```

This will publish the package with the base version from the `package.json` file.

## Examples

### Publishing a development version

```sh
npm run publish:webapis
```

Output:

```
Publishing @yodaos-jsar/web-apis@0.7.0-abc123
```

### Releasing a new version

```sh
npm run publish:webapis -- --release
```

Output:

```
Publishing @yodaos-jsar/web-apis@0.7.0
```
