# Programming Languages

There are several programming languages that we used in this project, we know that it might be hard to understand this decision, but we have our reasons, and we will explain them in this document, and define the rules that we follow in selecting the programming languages.

## Selected Programming Languages

We selected the following programming languages to be used in this project:

- C/C++
- Rust
- TypeScript

We use C/C++ for the main part of the project, including the main components such as the renderer, media, content manager and other services, the reason behind this is that C/C++ is most convenient for integrating with the other applications and 3d engines.

And in client-side, we use C/C++ to bootstrap the Node.js as the script thread, and expose the Web APIs to the JavaScript via the Node.js C++ Addons, the reason is also the Node.js and v8 has used C++ as its API language.

Rust has its usecases in the JSAR project. In Rust ecosystem, Mozila's [servo](https://github.com/servo/servo) project has achieved a lot of crates that can be used in building a Web browser especially the CSS parser and layout engine, these crates are the main reason why we include Rust to build JSAR.

As for TypeScript, it has been adopted as the default scripting language for Deno, Bun and Node.js, and there is still a project: AssemblyScript that compiles TypeScript-like code to WebAssembly for speed and performance, so we decided to use TypeScript as the scripting language for the project.

## Language Rules

This section will define some rules that we follow in selecting the programming language when we need to add a new feature or component to the project.

**The less scripting language, the better of the performance**

We prefer to use the less scripting language as possible, because the scripting language (including WebAssembly) has a performance overhead at bootstrap and runtime, so we will keep the scripting language as less as possible.

**TypeScript should only call native from the C/C++**

Both the Rust and C/C++ languages could expose the JavaScript APIs via N-API, to make the architecture more consistent, we will only allow TypeScript to call the native APIs from the C/C++, it means that we can't use N-API with Rust.

**Rust should be restricted to add global states**

Rust is the programming language that is designed to be safe and concurrent, using Rust to add global states will make the project more complex and hard to maintain, thus we need to restrict the Rust not to add global states except for resources.
