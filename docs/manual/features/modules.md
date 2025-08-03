# Modules

JSAR implemented [ECMAScript modules][esm] to manage the dependencies between scripts, which is the official standard to write modular JavaScript, and it's supported by all modern browsers and JavaScript runtimes.

Based on the basic implementation of the module system, JSAR provides many other module loaders for the different formats, such as: JSON, image, audio, video, binary files, etc. Both they are useful for space development.

## Getting started

To use ECMAScript modules in JSAR:

```ts title="foo.ts"
export function foo() {
  console.log('foo');
}
```

Then use a `<script>` tag to import the `.ts` file:

```html title="index.html"
<html>
<head>
  <title>Modules</title>
  <script type="module">
    import { foo } from './foo.ts';
    foo();  // prints 'foo'
  </script>
</head>
<body>
</body>
</html>
```

## Module declaration

To declare a module, ECMAScript modules provide the `export` keyword to export the module members:

```ts title="foo.ts"
export function foo() {
  console.log('foo');
}

export default function bar() {
  console.log('bar');
}
```

Export a constant:

```ts title="baz.ts"
export const baz = 'baz';
```

And `export default` to export the default member:

```ts title="bar.ts"
export default function bar() {
  console.log('bar');
}
```

## Importing modules

To import the module exported by `export`:

```ts title="main.ts"
import { foo } from './foo.ts';
foo();  // prints 'foo'
```

To import the constant exported by `export const`:

```ts title="main.ts"
import { baz } from './baz.ts';
console.log(baz);  // prints 'baz'
```

To import the default member exported by `export default`:

```ts title="main.ts"
import bar1 from './bar.ts';
import bar2 from './bar.ts';
bar1();  // prints 'bar'
bar2();  // prints 'bar'
```

Note that local import specifiers must always include the full file extension:

```ts title="main.ts"
import { foo } from './foo';  // Error: missing file extension
```

## Importing third-party modules

To import a third-party module, you just need to use the `import` but with a URL specifier:

```ts title="main.ts"
import { foo } from 'https://example.com/foo.ts';
```

> We are planning to support importing third-party modules from the package registries like [NPM][npm] and [JSR][] such as: `npm:cowsay@1.6.0` and `jsr:@luca/cases@1.0.0`.

## Using `importmap` to manage dependencies

Importmap is a JSON file that maps the module specifiers to the URLs, which is useful to manage the dependencies in the application.

To use the importmap, you need to add a `<script>` tag with the `type="importmap"`:

```html title="index.html"
<html>
<head>
  <title>Modules</title>
  <script type="importmap">
    {
      "imports": {
        "three": "https://ar.rokidcdn.com/web-assets/yodaos-jsar/dist/three/build/three.module.js",
        "three/addons/": "https://ar.rokidcdn.com/web-assets/yodaos-jsar/dist/three/examples/jsm/"
      }
    }
  </script>
  <script type="module">
    import * as THREE from 'three';
    import { PDBLoader } from 'three/addons/loaders/PDBLoader.js';

    console.info(THREE); // prints the THREE object
    console.info(PDBLoader); // prints the PDBLoader object
  </script>
</head>
```

The `three` and `three/addons/loaders/PDBLoader.js` will be resolved to the URLs from the importmap:

| Module specifier | URL |
| --- | --- |
| `three` | https://ar.rokidcdn.com/web-assets/yodaos-jsar/dist/three/build/three.module.js |
| `three/addons/loaders/PDBLoader.js` | https://ar.rokidcdn.com/web-assets/yodaos-jsar/dist/three/examples/jsm/loaders/PDBLoader.js |

Note: the importmap works for the whole document, the importmap is shared between all the modules in the document, including the modules imported by `<script>` tags.

## Asset loaders

There are assets loaders to load the different formats of the modules.

| Loader      | Description | Specifier extensions | Export type |
| ----------- | ----------- | -------------------- | ----------- |
| JSON        | Load the JSON file        | `.json` | `object` |
| WebAssembly | Load the WebAssembly file | `.wasm` | [`ArrayBuffer`][ArrayBuffer] |
| Image       | Load the image file       | `.jpeg`, `.png`, `.svg`, `.gif`  | [`ArrayBuffer`][ArrayBuffer] |
| Audio       | Load the audio file       | `.mp3`, `.wav`, `.ogg`  | [`ArrayBuffer`][ArrayBuffer] |
| Binary      | Load a binary file        | `.bin`, `.data`  | [`ArrayBuffer`][ArrayBuffer] |

> For safety reasons, the loaders except the JSON loader will return the assets as [`ArrayBuffer`][ArrayBuffer], the developers need to decode the assets by themselves, but we'll consider returning the decoded object such as `Image` and `Audio` in the future versions once the safety is guaranteed.
>
> If you are eager for this feature, please let us know at [Rokid Forum](https://forum.rokid.com/index).

For instance, to load a JSON file:

```json title="data.json"
{
  "foo": "bar"
}
```

```ts title="main.ts"
import data from './data.json';
console.info(data); // prints { foo: 'bar' }
```

The JSON loader will load the JSON file and parse it as an JavaScript object.

To load other assets that returns an [`ArrayBuffer`][ArrayBuffer]:

```ts title="main.ts"
import screenshot from './screenshot.png';
import music from './music.mp3';
console.info(screenshot); // prints the screenshot as ArrayBuffer
console.info(music); // prints the music as ArrayBuffer
```

To help developers to load custom assets that are not supported by the built-in loaders, JSAR provides two binary loaders: binary and data, developers can rename the custom assets to `.bin` or `.data` to use with the binary loaders.

```text title="names.data"
Alice
Bob
Chrome
...
Yorkie
Zoe
```

```sh title="data.bin"
[...]
```

```ts title="main.ts"
import names from './names.data';
console.info(names); // prints the names as ArrayBuffer

import data from './data.bin';
console.info(data); // prints the data as ArrayBuffer
```

[ArrayBuffer]: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/ArrayBuffer

## Dynamic import

Dynamic import is a feature that allows you to import modules (including assets) dynamically at runtime, which is useful to load the modules or assets conditionally.

To use dynamic import, you need to use the `import()` function instead of the `import` statement:

```ts title="main.ts"
async function load() {
  const module = await import('./foo.ts');
  module.foo();  // prints 'foo'

  const screenshot = await import('./screenshot.png');
  console.info(screenshot); // prints the screenshot as ArrayBuffer
}
```

[esm]: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Modules
[npm]: https://www.npmjs.com/
[jsr]: https://jsr.io/
