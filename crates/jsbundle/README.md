# jsbundle

This crate bundles the JavaScript sources and includes them in a static library, which is then linked into the core library.

## Usage

In C/C++ code, you can get the uncompressed JavaScript source code:

```c
void* bootstrapSrcPtr = get_jsbootstrap_ptr();
void* coreSrcPtr = get_jsbundle_ptr();
```
