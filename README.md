# JSAR Runtime

The JSAR runtime SDK for embedding in Unity, UnrealEngine, RealityKit and others.

## Getting Started

TODO

## Development

### Prerequisites

- [Node.js](https://nodejs.org/en/) v18.16.1
- [Rust](https://www.rust-lang.org/) v1.70.0 or later

And you need to add the following targets to your Rust toolchain:

```sh
$ rustup target add aarch64-linux-android
$ rustup target add aarch64-apple-darwin
$ rustup target add x86_64-apple-darwin
$ rustup target add x86_64-pc-windows-msvc
```

### Build from source

Firstly, you need to install the dependencies and build the JSAR JavaScript bundle which is used in the runtime.

```sh
$ npm install && make jsbundle
```

Then you can build the runtime library:

```sh
$ make darwin # for macOS universal library
$ make android # for Android aarch64 library
$ make windows # for Windows x86_64 library
```

If you want to do a clean build, you can run:

```sh
$ make darwin clean=yes
```

## Loaders

- [jsar-loader-unity](https://github.com/M-CreativeLab/jsar-loader-unity)
- jsar-loader-unreal
- jsar-loader-ios
- jsar-loader-android

## License

[Apache License 2.0](LICENSE)
