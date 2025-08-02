# TypeScript support

In JSAR, [TypeScript][] is the recommended scripting language, as it provides type checking in the development process. This section will introduce why TypeScript is recommended, the basics of TypeScript, and how to use TypeScript in JSAR.

## Why TypeScript

In the three-dimensional development field, object relationships are more complex. Therefore, type checking is essential for JavaScript.

[TypeScript][], as a superset of JavaScript, provides type checking, allowing us to discover errors earlier in the development process, thus improving development efficiency. Additionally, TypeScript being a superset of JavaScript means that it supports JavaScript as well, making it a good choice for developers who may not want to use TypeScript.

## TypeScript Basics

If you are new to TypeScript, you can learn the basics of TypeScript on the [TypeScript official website][TypeScript]. This section will not go into detail on TypeScript basics, but will provide a brief introduction to use TypeScript in JSAR.

### Zero configuration

To get started with TypeScript in JSAR, you don't need to configure anything. Just write TypeScript code in your XSML document, and JSAR will automatically strip the type annotations and compile the TypeScript code to JavaScript.

> JSAR uses the [swc][] compiler and its function [`swc_fast_ts_strip::operate()`](https://rustdoc.swc.rs/swc_fast_ts_strip/fn.operate.html) for TypeScript compilation, which is fast enough at runtime, so you don't need to worry about the performance of TypeScript compilation.

[swc]: https://swc.rs/

### Type checking

With TypeScript, you can define the type of variables, functions, and objects.

```typescript
class Foo {
  bar: string = 'bar';
  constructor() {
    console.log(this.bar, 'from TypeScript');
  }
}
```

And you can get type checking errors when you assign the wrong type to a variable.

```typescript
const foo = new Foo();
foo.bar = 1; // Error: Type 'number' is not assignable to type 'string'.
```

## Using TypeScript in JSAR

TypeScript in JSAR can work on HTML and XSML documents, but different implementations. 

### Providing declaration file

To use untyped JavaScript libraries in TypeScript, you are able to provide the declaration files.

> Thanks to [Deno Docs](https://docs.deno.com/runtime/fundamentals/typescript/#providing-declaration-files) for the idea of providing declaration files in the source.

#### Providing types in the source

To provide types in the source, you can use the `@ts-self-types` directive:

```js title="add.js"
// @ts-self-types="./add.d.ts"

export function add(a, b) {
  return a + b;
}
```

```ts title="add.d.ts"
export function add(a: number, b: number): number;
```

#### Providing types in the importer

Another way to provide types is to use the `@ts-types` directive:

```ts title="main.ts"
// @ts-types="./add.d.ts"
import { add } from './add.js';
```

### Importing TypeScript module

To use TypeScript, you need to create a `.ts` file:

```ts title="main.ts"
class Foo {
  bar: string = 'bar';
  constructor() {
    console.log(this.bar, 'from TypeScript');
  }
}
new Foo();
```

Then use a `<script>` tag to import the `.ts` file:

```html
<script src="./main.ts" type="module"></script>
```

Or use the `import` from JavaScript:

```js title="main.js"
import './main.ts';
```

Of course you can also use the `import` the TypeScript module in `<script>` tag:

```html
<script type="module">
  import './main.ts';
</script>
```

### TypeScript in `<script>`

TypeScript cannot be used in a `<script>` tag:

```html
<script type="module">
  class Foo {
    bar: string = 'bar';
    constructor() {
      console.log(this.bar, 'from TypeScript');
    }
  }
</script>
```

In the above syntax, we have no way to tell JSAR the script is TypeScript, so JSAR will keep the backward compatibility and treat the script as JavaScript by default.

[TypeScript]: https://www.typescriptlang.org/
