# GLSL Attribute Parser

This document describes the GLSL attribute parser implementation in JSAR Runtime, which allows parsing vertex shader source code to extract attribute metadata.

## Overview

The GLSL attribute parser uses the [glsl-lang](https://github.com/alixinne/glsl-lang) Rust crate to parse GLSL vertex shaders and extract information about vertex attributes (their names, types, and locations). This information is exposed to C++ via FFI for use in WebGL shader/program management.

## Features

- ✅ Supports GLSL 100 ES (`attribute` keyword) and GLSL 300 ES (`in` keyword)
- ✅ Handles explicit `layout(location = N)` qualifiers
- ✅ Auto-assigns locations based on declaration order when not explicitly specified
- ✅ Compatible with WebGL 1.0 and WebGL 2.0 shaders
- ✅ Full FFI interface for C++ integration

## Rust API

### Data Structures

```rust
/// Represents metadata about a vertex attribute in GLSL
pub struct GLSLAttribute {
  pub name: String,      // Attribute name (e.g., "position", "normal")
  pub type_name: String, // GLSL type (e.g., "vec3", "vec4", "mat4")
  pub location: i32,     // Assigned location (0-based index)
}

/// Parser for extracting vertex attributes from GLSL source code
pub struct GLSLAttributeParser {
  // ...
}
```

### Methods

```rust
impl GLSLAttributeParser {
  /// Create a new parser
  pub fn new() -> Self;
  
  /// Parse GLSL source and extract all attribute declarations
  pub fn parse(&mut self, source: &str) -> Result<(), String>;
  
  /// Get the parsed attributes
  pub fn get_attributes(&self) -> &[GLSLAttribute];
  
  /// Find attribute location by name
  pub fn get_attrib_location(&self, name: &str) -> Option<i32>;
}
```

### FFI Functions

```rust
/// Parse GLSL source and return all attributes
fn parse_glsl_attributes(source: &str) -> Result<GLSLAttributeList, String>;

/// Get attribute location by name
fn get_glsl_attrib_location(source: &str, name: &str) -> Result<i32, String>;
```

## C++ API

### Include

```cpp
#include <crates/bindings.webgl.hpp>
```

### Data Structures

```cpp
namespace crates::webgl {
  struct GLSLAttribute {
    std::string name;      // Attribute name
    std::string type_name; // Attribute type
    int32_t location;      // Attribute location
  };
}
```

### GLSLAttributeParser Class

```cpp
namespace crates::webgl {
  class GLSLAttributeParser {
  public:
    /// Parse GLSL vertex shader source and extract all attribute declarations
    static std::vector<GLSLAttribute> ParseAttributes(const std::string &source);
    
    /// Get the location of a specific attribute by name
    static std::optional<int32_t> GetAttribLocation(const std::string &source, 
                                                     const std::string &name);
  };
}
```

## Usage Examples

### C++ Examples

#### Example 1: Parse All Attributes

```cpp
#include <crates/bindings.webgl.hpp>
#include <iostream>

std::string vertexShader = R"(
  #version 300 es
  precision highp float;
  
  in vec3 position;
  in vec3 normal;
  in vec2 uv;
  
  uniform mat4 modelViewMatrix;
  uniform mat4 projectionMatrix;
  
  void main() {
    gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
  }
)";

// Parse all attributes
auto attributes = crates::webgl::GLSLAttributeParser::ParseAttributes(vertexShader);

std::cout << "Found " << attributes.size() << " attributes:" << std::endl;
for (const auto& attr : attributes) {
  std::cout << "  - " << attr.name 
            << " (" << attr.type_name << ")" 
            << " at location " << attr.location << std::endl;
}

// Output:
// Found 3 attributes:
//   - position (vec3) at location 0
//   - normal (vec3) at location 1
//   - uv (vec2) at location 2
```

#### Example 2: Get Specific Attribute Location

```cpp
#include <crates/bindings.webgl.hpp>
#include <iostream>

std::string vertexShader = R"(
  #version 300 es
  layout(location = 0) in vec3 aPos;
  layout(location = 1) in vec3 aNormal;
  
  void main() {
    gl_Position = vec4(aPos, 1.0);
  }
)";

// Get location of a specific attribute
auto location = crates::webgl::GLSLAttributeParser::GetAttribLocation(vertexShader, "aPos");

if (location.has_value()) {
  std::cout << "aPos is at location " << location.value() << std::endl;
} else {
  std::cout << "aPos not found" << std::endl;
}

// Output: aPos is at location 0
```

#### Example 3: Integration with WebGL Program

```cpp
#include <crates/bindings.webgl.hpp>
#include <client/graphics/webgl_program.hpp>

void setupWebGLProgram(client_graphics::WebGLProgram* program, 
                       const std::string& vertexShaderSource) {
  try {
    // Parse attributes from shader source
    auto attributes = crates::webgl::GLSLAttributeParser::ParseAttributes(vertexShaderSource);
    
    // Set attribute locations in the program
    for (const auto& attr : attributes) {
      program->setAttribLocation(attr.name, attr.location);
    }
    
    std::cout << "Successfully set up " << attributes.size() << " attributes" << std::endl;
  } 
  catch (const std::exception& e) {
    std::cerr << "Failed to parse shader attributes: " << e.what() << std::endl;
  }
}
```

### Rust Examples

#### Example 1: Direct Parser Usage

```rust
use crate::webgl::GLSLAttributeParser;

let shader_source = r#"
  #version 300 es
  in vec3 position;
  in vec3 normal;
  
  void main() {
    gl_Position = vec4(position, 1.0);
  }
"#;

let mut parser = GLSLAttributeParser::new();
parser.parse(shader_source).expect("Failed to parse shader");

for attr in parser.get_attributes() {
  println!("{} ({}) at location {}", attr.name, attr.type_name, attr.location);
}
```

#### Example 2: FFI Function Usage

```rust
use crate::webgl::{parse_glsl_attributes, get_glsl_attrib_location};

let shader_source = "...";

// Get all attributes
let result = parse_glsl_attributes(shader_source)?;
for attr in &result.attributes {
  println!("{}: {}", attr.name, attr.location);
}

// Get specific attribute
let location = get_glsl_attrib_location(shader_source, "position")?;
println!("position is at {}", location);
```

## Supported GLSL Types

The parser recognizes the following GLSL types:

### Scalar Types
- `float`, `double`
- `int`, `uint`
- `bool`

### Vector Types
- `vec2`, `vec3`, `vec4` (float vectors)
- `dvec2`, `dvec3`, `dvec4` (double vectors)
- `ivec2`, `ivec3`, `ivec4` (integer vectors)
- `uvec2`, `uvec3`, `uvec4` (unsigned integer vectors)
- `bvec2`, `bvec3`, `bvec4` (boolean vectors)

### Matrix Types
- `mat2`, `mat3`, `mat4` (square matrices)
- `mat2x3`, `mat2x4`, `mat3x2`, `mat3x4`, `mat4x2`, `mat4x3` (non-square matrices)
- `dmat2`, `dmat3`, `dmat4` (double matrices)
- `dmat2x3`, `dmat2x4`, `dmat3x2`, `dmat3x4`, `dmat4x2`, `dmat4x3` (double non-square matrices)

## Location Assignment Rules

1. **Explicit Layout**: If an attribute has `layout(location = N)`, that location is used
2. **Auto-assignment**: If no explicit location, attributes are assigned locations in the order they appear in the source code, starting from 0
3. **Mixed Mode**: You can mix explicit and auto-assigned locations; auto-assigned locations continue from the highest explicit location + 1

### Examples

```glsl
// All auto-assigned
in vec3 position;  // location 0
in vec3 normal;    // location 1
in vec2 uv;        // location 2

// All explicit
layout(location = 2) in vec3 position;  // location 2
layout(location = 0) in vec3 normal;    // location 0
layout(location = 1) in vec2 uv;        // location 1

// Mixed (auto-assigned locations continue after highest explicit)
layout(location = 5) in vec3 position;  // location 5
in vec3 normal;                         // location 0
in vec2 uv;                             // location 1
```

## Testing

The implementation includes comprehensive tests covering:

- Basic attribute parsing (GLSL 300 ES with `in` qualifier)
- Explicit layout location parsing
- GLSL 100 ES compatibility (`attribute` qualifier)
- Attribute lookup by name
- FFI function correctness
- Error handling

Run tests with:
```bash
cargo test --package jsar_jsbindings --lib webgl::tests
```

## Implementation Details

The parser uses the Visitor pattern from glsl-lang to walk the GLSL AST:

1. Parse GLSL source using `glsl_lang_pp::processor::fs::StdProcessor`
2. Implement `Visitor` trait to visit `SingleDeclaration` nodes
3. Check for attribute/in qualifiers in type qualifiers
4. Extract attribute name and type from the declaration
5. Check for explicit layout location or auto-assign
6. Store attributes in order for later retrieval

## Related Files

- **Rust Implementation**: `crates/jsbindings/webgl.rs`
- **C++ Wrapper**: `crates/jsbindings/bindings.webgl.hpp`
- **Generated FFI Header**: `crates/jsbindings/holocron_webgl.autogen.hpp`
- **Build Script**: `crates/jsbindings/build.rs`
- **Tests**: `crates/jsbindings/webgl.rs` (test module)

## WebGL Specification References

- [WebGL 1.0 Specification - getAttribLocation](https://www.khronos.org/registry/webgl/specs/latest/1.0/#5.14.6)
- [GLSL ES 1.00 Specification](https://www.khronos.org/files/opengles_shading_language.pdf)
- [GLSL ES 3.00 Specification](https://www.khronos.org/registry/OpenGL/specs/es/3.0/GLSL_ES_Specification_3.00.pdf)

## Future Enhancements

Potential improvements for future versions:

- [ ] Cache parsed results to avoid re-parsing the same shader
- [ ] Support for more complex attribute types (structs, arrays)
- [ ] Integration with shader compilation error reporting
- [ ] Performance optimizations for large shaders
- [ ] Support for geometry shader attributes (if needed)
