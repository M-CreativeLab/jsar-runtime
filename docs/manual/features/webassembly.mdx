# WebAssembly

The [WebAssembly][] (WASM) is available in JSAR, which allows developers to write high-performance code in C/C++ and Rust for the application development.

[WebAssembly]: https://webassembly.org/

## Using WebAssembly in JSAR

JSAR's ECMAScript modules are more friendly to WebAssembly, you can import the WebAssembly module directly in your code.

Assuming you have a WebAssembly module `add.wasm`:

```c title="add.c"
int add(int a, int b) {
  return a + b;
}
```

You can compile the `add.c` to `add.wasm` with the following command:

```sh
emcc add.c -o add.wasm
```

Then you can import the `add.wasm` in your JavaScript code:

```js
import code from './add.wasm';

const wasmModule = new WebAssembly.Module(code);
const wasmInstance = new WebAssembly.Instance(wasmModule);
console.log(wasmInstance.exports.add(1, 2)); // 3
```

Or you can use the streaming compilation:

```js
const { instance, module } = await WebAssembly.instantiateStreaming(import('./add.wasm'));
console.log(instance.exports.add(1, 2)); // 3
```

## References

- [WebAssembly](https://webassembly.org/)
- [Emscripten](https://emscripten.org/)
