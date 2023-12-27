# JSAR Runtime

The JSAR runtime SDK for embedding in Unity, UnrealEngine, RealityKit and others.

## Getting Started

TODO

## Development

### Prerequisites

- [Node.js](https://nodejs.org/en/) v18.16.1
- [Rust](https://www.rust-lang.org/) v1.70.0 or later

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

## Loaders

- [jsar-loader-unity](https://github.com/M-CreativeLab/jsar-loader-unity)
- jsar-loader-unreal
- jsar-loader-ios
- jsar-loader-android

## License

[Apache License 2.0](LICENSE)
