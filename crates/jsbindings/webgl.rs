use glsl_lang::ast;
use glsl_lang::visitor::{Host, HostMut, Visit, Visitor, VisitorMut};
use serde::{Deserialize, Serialize};
use std::path::Path;

use crate::glsl_transpiler;

struct MyGLSLPatcher {}

impl MyGLSLPatcher {
  fn create_model_view_matrix_expr(&self) -> ast::Expr {
    let new_lhs: ast::Expr =
      ast::ExprData::Variable(ast::IdentifierData(ast::SmolStr::new_inline("viewMatrix")).into())
        .into();
    let new_rhs: ast::Expr =
      ast::ExprData::Variable(ast::IdentifierData(ast::SmolStr::new_inline("modelMatrix")).into())
        .into();
    let new_binary_expr: ast::Expr = ast::ExprData::Binary(
      ast::BinaryOpData::Mult.into(),
      Box::new(new_lhs),
      Box::new(new_rhs),
    )
    .into();
    new_binary_expr
  }

  fn handle_expr(&self, expr: &mut ast::Expr) -> bool {
    match &mut expr.content {
      ast::ExprData::Variable(identifier) => {
        if identifier.content.0 == "modelViewMatrix" {
          *expr = self.create_model_view_matrix_expr();
          true
        } else {
          false
        }
      }
      ast::ExprData::Unary(_, operand) => self.handle_expr(operand),
      ast::ExprData::Binary(_, lhs, rhs) => {
        let r1 = self.handle_expr(lhs);
        let r2 = self.handle_expr(rhs);
        r1 || r2
      }
      ast::ExprData::Assignment(_, _, rhs) => self.handle_expr(rhs),
      ast::ExprData::FunCall(_, args) => {
        let mut changed = false;
        for arg in args {
          changed |= self.handle_expr(arg);
        }
        changed
      }
      _ => false,
    }
  }
}

impl VisitorMut for MyGLSLPatcher {
  fn visit_expr(&mut self, expr: &mut ast::Expr) -> Visit {
    if self.handle_expr(expr) {
      Visit::Parent
    } else {
      Visit::Children
    }
  }
}

fn patch_glsl_source_from_str(s: &str) -> String {
  use glsl_lang::{
    ast::TranslationUnit, lexer::full::fs::PreprocessorExt, parse::IntoParseBuilderExt,
  };

  let mut processor = glsl_lang_pp::processor::fs::StdProcessor::new();
  let mut tu: TranslationUnit = processor
    .open_source(s, Path::new("."))
    .builder()
    .parse()
    .map(|(mut tu, _, iter)| {
      iter.into_directives().inject(&mut tu);
      tu
    })
    .expect(format!("Failed to parse GLSL source: \n{}\n", s).as_str());

  let mut my_glsl_patcher = MyGLSLPatcher {};
  tu.visit_mut(&mut my_glsl_patcher);

  {
    /*
     * This reorders the preprocessor directives in the GLSL source code.
     *
     * 1. Move the #version directive to the top.
     * 2. Move the #extension directives to the top after the #version directive if exists.
     */
    let mut versions_list = Vec::new();
    let mut extensions_list = Vec::new();
    tu.0.retain(|decl| match &decl.content {
      ast::ExternalDeclarationData::Preprocessor(processor) => match processor.content {
        ast::PreprocessorData::Version(_) => {
          versions_list.push(decl.clone());
          false
        }
        ast::PreprocessorData::Extension(_) => {
          extensions_list.push(decl.clone());
          false
        }
        _ => true,
      },
      _ => true,
    });
    tu.0.splice(0..0, extensions_list);
    tu.0.splice(0..0, versions_list);
  }

  let mut s = String::new();
  glsl_transpiler::glsl::show_translation_unit(
    &mut s,
    &tu,
    glsl_transpiler::glsl::FormattingState::default(),
  )
  .expect("Failed to show GLSL");
  s
}

/// Visitor to collect all variable references in the shader
struct ReferenceCollector {
  referenced_names: std::collections::HashSet<String>,
}

impl ReferenceCollector {
  fn new() -> Self {
    Self {
      referenced_names: std::collections::HashSet::new(),
    }
  }
}

impl Visitor for ReferenceCollector {
  fn visit_expr(&mut self, expr: &ast::Expr) -> Visit {
    // Check if this expression is a variable reference
    if let ast::ExprData::Variable(identifier) = &expr.content {
      self.referenced_names.insert(identifier.content.0.to_string());
    }
    Visit::Children
  }
}

/// Represents metadata about a vertex attribute in GLSL
#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
pub struct GLSLAttribute {
  /// The name of the attribute (e.g., "position", "normal")
  pub name: String,
  /// The GLSL type of the attribute (e.g., "vec3", "vec4", "mat4")
  pub type_name: String,
  /// The assigned location of the attribute (0-based index)
  pub location: i32,
}

/// Parser for extracting vertex attributes from GLSL source code
pub struct GLSLAttributeParser {
  attributes: Vec<GLSLAttribute>,
  next_location: i32,
  referenced_names: std::collections::HashSet<String>,
}

impl GLSLAttributeParser {
  /// Create a new parser
  pub fn new() -> Self {
    Self {
      attributes: Vec::new(),
      next_location: 0,
      referenced_names: std::collections::HashSet::new(),
    }
  }

  /// Parse GLSL source and extract all attribute declarations
  pub fn parse(&mut self, source: &str) -> Result<(), String> {
    use glsl_lang::{
      ast::TranslationUnit, lexer::full::fs::PreprocessorExt, parse::IntoParseBuilderExt,
    };

    let mut processor = glsl_lang_pp::processor::fs::StdProcessor::new();
    let tu: TranslationUnit = processor
      .open_source(source, Path::new("."))
      .builder()
      .parse()
      .map(|(mut tu, _, iter)| {
        iter.into_directives().inject(&mut tu);
        tu
      })
      .map_err(|e| format!("Failed to parse GLSL source: {:?}", e))?;

    // First pass: collect all variable references
    let mut ref_collector = ReferenceCollector::new();
    tu.visit(&mut ref_collector);
    self.referenced_names = ref_collector.referenced_names;

    // Second pass: visit the AST to find attributes
    use glsl_lang::visitor::Host;
    tu.visit(self);

    // Filter out unreferenced attributes (inactive attributes)
    self.attributes.retain(|attr| self.referenced_names.contains(&attr.name));

    Ok(())
  }

  /// Get the parsed attributes
  pub fn get_attributes(&self) -> &[GLSLAttribute] {
    &self.attributes
  }

  /// Find attribute location by name
  pub fn get_attrib_location(&self, name: &str) -> Option<i32> {
    self
      .attributes
      .iter()
      .find(|attr| attr.name == name)
      .map(|attr| attr.location)
  }

  /// Extract type name from TypeSpecifierNonArray
  fn type_to_string(&self, ty: &ast::TypeSpecifierNonArray) -> String {
    match &ty.content {
      ast::TypeSpecifierNonArrayData::Float => "float".to_string(),
      ast::TypeSpecifierNonArrayData::Double => "double".to_string(),
      ast::TypeSpecifierNonArrayData::Int => "int".to_string(),
      ast::TypeSpecifierNonArrayData::UInt => "uint".to_string(),
      ast::TypeSpecifierNonArrayData::Bool => "bool".to_string(),
      ast::TypeSpecifierNonArrayData::Vec2 => "vec2".to_string(),
      ast::TypeSpecifierNonArrayData::Vec3 => "vec3".to_string(),
      ast::TypeSpecifierNonArrayData::Vec4 => "vec4".to_string(),
      ast::TypeSpecifierNonArrayData::DVec2 => "dvec2".to_string(),
      ast::TypeSpecifierNonArrayData::DVec3 => "dvec3".to_string(),
      ast::TypeSpecifierNonArrayData::DVec4 => "dvec4".to_string(),
      ast::TypeSpecifierNonArrayData::BVec2 => "bvec2".to_string(),
      ast::TypeSpecifierNonArrayData::BVec3 => "bvec3".to_string(),
      ast::TypeSpecifierNonArrayData::BVec4 => "bvec4".to_string(),
      ast::TypeSpecifierNonArrayData::IVec2 => "ivec2".to_string(),
      ast::TypeSpecifierNonArrayData::IVec3 => "ivec3".to_string(),
      ast::TypeSpecifierNonArrayData::IVec4 => "ivec4".to_string(),
      ast::TypeSpecifierNonArrayData::UVec2 => "uvec2".to_string(),
      ast::TypeSpecifierNonArrayData::UVec3 => "uvec3".to_string(),
      ast::TypeSpecifierNonArrayData::UVec4 => "uvec4".to_string(),
      ast::TypeSpecifierNonArrayData::Mat2 => "mat2".to_string(),
      ast::TypeSpecifierNonArrayData::Mat3 => "mat3".to_string(),
      ast::TypeSpecifierNonArrayData::Mat4 => "mat4".to_string(),
      ast::TypeSpecifierNonArrayData::Mat23 => "mat2x3".to_string(),
      ast::TypeSpecifierNonArrayData::Mat24 => "mat2x4".to_string(),
      ast::TypeSpecifierNonArrayData::Mat32 => "mat3x2".to_string(),
      ast::TypeSpecifierNonArrayData::Mat34 => "mat3x4".to_string(),
      ast::TypeSpecifierNonArrayData::Mat42 => "mat4x2".to_string(),
      ast::TypeSpecifierNonArrayData::Mat43 => "mat4x3".to_string(),
      ast::TypeSpecifierNonArrayData::DMat2 => "dmat2".to_string(),
      ast::TypeSpecifierNonArrayData::DMat3 => "dmat3".to_string(),
      ast::TypeSpecifierNonArrayData::DMat4 => "dmat4".to_string(),
      ast::TypeSpecifierNonArrayData::DMat23 => "dmat2x3".to_string(),
      ast::TypeSpecifierNonArrayData::DMat24 => "dmat2x4".to_string(),
      ast::TypeSpecifierNonArrayData::DMat32 => "dmat3x2".to_string(),
      ast::TypeSpecifierNonArrayData::DMat34 => "dmat3x4".to_string(),
      ast::TypeSpecifierNonArrayData::DMat42 => "dmat4x2".to_string(),
      ast::TypeSpecifierNonArrayData::DMat43 => "dmat4x3".to_string(),
      ast::TypeSpecifierNonArrayData::TypeName(tn) => tn.content.0.to_string(),
      _ => "unknown".to_string(),
    }
  }

  /// Extract explicit location from layout qualifier if present
  fn extract_layout_location(&self, qualifiers: &[ast::LayoutQualifierSpec]) -> Option<i32> {
    for qualifier in qualifiers {
      match &qualifier.content {
        ast::LayoutQualifierSpecData::Identifier(ident, Some(expr)) => {
          if ident.content.0.as_str() == "location" {
            // Try to extract the constant expression value
            if let ast::ExprData::IntConst(val) = &expr.content {
              return Some(*val);
            }
          }
        }
        _ => {}
      }
    }
    None
  }

  /// Check if a declaration has an attribute/in qualifier
  fn has_attribute_qualifier(&self, qualifiers: &[ast::TypeQualifierSpec]) -> (bool, Option<i32>) {
    let mut is_attribute = false;
    let mut layout_location = None;

    for qualifier in qualifiers {
      match &qualifier.content {
        ast::TypeQualifierSpecData::Storage(storage) => match &storage.content {
          ast::StorageQualifierData::Attribute | ast::StorageQualifierData::In => {
            is_attribute = true;
          }
          _ => {}
        },
        ast::TypeQualifierSpecData::Layout(layout) => {
          layout_location = self.extract_layout_location(&layout.content.ids);
        }
        _ => {}
      }
    }

    (is_attribute, layout_location)
  }
}

impl Visitor for GLSLAttributeParser {
  fn visit_single_declaration(&mut self, declaration: &ast::SingleDeclaration) -> Visit {
    // Check if this is an attribute declaration
    if let Some(ref qualifier) = declaration.ty.qualifier {
      let (is_attribute, layout_location) =
        self.has_attribute_qualifier(&qualifier.content.qualifiers);

      if is_attribute {
        if let Some(ref name) = declaration.name {
          // Extract type name from the type specifier
          let type_name = self.type_to_string(&declaration.ty.ty.ty);

          let location = layout_location.unwrap_or_else(|| {
            let loc = self.next_location;
            self.next_location += 1;
            loc
          });

          self.attributes.push(GLSLAttribute {
            name: name.content.0.to_string(),
            type_name,
            location,
          });
        }
      }
    }

    Visit::Children
  }
}

#[cxx::bridge(namespace = "holocron::webgl")]
mod ffi {
  extern "Rust" {
    /// Patch GLSL source code from string
    #[cxx_name = "patchGLSLSourceFromStr"]
    fn patch_glsl_source_from_str(input: &str) -> String;

    /// Parse GLSL vertex shader source and extract attributes as JSON
    #[cxx_name = "parseGLSLAttributes"]
    fn parse_glsl_attributes(source: &str) -> String;
  }
}

/// Parse GLSL source and return all attributes as JSON string
fn parse_glsl_attributes(source: &str) -> String {
  let mut parser = GLSLAttributeParser::new();
  
  match parser.parse(source) {
    Ok(_) => {
      let attributes = parser.get_attributes();
      serde_json::to_string(attributes).unwrap_or_else(|_| "[]".to_string())
    }
    Err(_) => "[]".to_string(),
  }
}

#[cfg(test)]
mod tests {
  use super::*;

  #[test]
  fn test_patch_glsl_source() {
    let source_str = r#"
#extension GL_OVR_multiview2 : enable
layout(num_views = 2) in;

#version 300 es
precision highp float;
highp float a = 1.0;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 0) out highp vec4 glFragColor;
#extension GL_OES_standard_derivatives : enable

void main() { 
  gl_FragColor = vec4(1, 1, 1, 1); 
}"#;
    let patched_source_str = patch_glsl_source_from_str(source_str);
    assert_eq!(
      patched_source_str,
      r#"#version 300 es
#extension GL_OVR_multiview2 : enable
#extension GL_OES_standard_derivatives : enable
layout(num_views = 2) in;
precision highp float;
highp float a = 1.;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 0) out highp vec4 glFragColor;
void main() {
    gl_FragColor = vec4(1, 1, 1, 1);
}
"#
    )
  }

  #[test]
  fn test_patch_glsl_source_threejs() {
    let source_str = r#"
#version 300 es
#extension GL_OVR_multiview2 : enable
layout(num_views = 2) in;
#define VIEW_ID gl_ViewID_OVR

uniform mat4 modelMatrix;
uniform mat4 viewMatrices[2];
uniform mat4 modelViewMatrices[2];

#define viewMatrix viewMatrices[VIEW_ID]
#define modelViewMatrix modelMatrix * viewMatrix

in vec3 position;
void main() {
  gl_Position = modelViewMatrix * vec4(position, 1.0);
}
  "#;
    let patched_source_str = patch_glsl_source_from_str(source_str);
    assert_eq!(
      patched_source_str,
      r#"#version 300 es
#extension GL_OVR_multiview2 : enable
layout(num_views = 2) in;
uniform mat4 modelMatrix;
uniform mat4 viewMatrices[2];
uniform mat4 modelViewMatrices[2];
in vec3 position;
void main() {
    gl_Position = modelMatrix * viewMatrices[gl_ViewID_OVR] * vec4(position, 1.);
}
"#
    )
  }

  #[test]
  #[ignore]
  fn test_patch_glsl_source_elif_expand() {
    let source_str = r#"
#version 300 es
#define CS1
#define CS2
#define CS3

vec3 test() {
#if defined(CS1)
  return vec3(1.0, 0.0, 0.0);
#elif defined(CS2)
  return vec3(2.0, 0.0, 0.0);
#elif defined(CS3)
  return vec3(3.0, 1.0, 0.0);
#else
  return vec3(0.0, 0.0, 1.0);
#endif
}
"#;
    let patched_source_str = patch_glsl_source_from_str(source_str);
    assert_eq!(
      patched_source_str,
      r#"#version 300 es
vec3 test() {
    return vec3(1., 0., 0.);
}
"#
    )
  }

  #[test]
  fn test_parse_glsl_attributes_basic() {
    let source_str = r#"
#version 300 es
precision highp float;

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform mat4 modelViewMatrix;

out vec3 vNormal;
out vec2 vUv;

void main() {
  gl_Position = modelViewMatrix * vec4(position, 1.0);
  vNormal = normal;
  vUv = uv;
}
"#;
    let mut parser = GLSLAttributeParser::new();
    parser.parse(source_str).expect("Failed to parse GLSL");

    let attributes = parser.get_attributes();
    assert_eq!(attributes.len(), 3);

    // Check first attribute
    assert_eq!(attributes[0].name, "position");
    assert_eq!(attributes[0].type_name, "vec3");
    assert_eq!(attributes[0].location, 0);

    // Check second attribute
    assert_eq!(attributes[1].name, "normal");
    assert_eq!(attributes[1].type_name, "vec3");
    assert_eq!(attributes[1].location, 1);

    // Check third attribute
    assert_eq!(attributes[2].name, "uv");
    assert_eq!(attributes[2].type_name, "vec2");
    assert_eq!(attributes[2].location, 2);
  }

  #[test]
  fn test_parse_glsl_attributes_with_layout() {
    let source_str = r#"
#version 300 es
precision highp float;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 vNormal;
out vec2 vTexCoord;

void main() {
  gl_Position = vec4(aPos, 1.0);
  vNormal = aNormal;
  vTexCoord = aTexCoord;
}
"#;
    let mut parser = GLSLAttributeParser::new();
    parser.parse(source_str).expect("Failed to parse GLSL");

    let attributes = parser.get_attributes();
    assert_eq!(attributes.len(), 3);

    assert_eq!(attributes[0].name, "aPos");
    assert_eq!(attributes[0].type_name, "vec3");
    assert_eq!(attributes[0].location, 0);

    assert_eq!(attributes[1].name, "aNormal");
    assert_eq!(attributes[1].type_name, "vec3");
    assert_eq!(attributes[1].location, 1);

    assert_eq!(attributes[2].name, "aTexCoord");
    assert_eq!(attributes[2].type_name, "vec2");
    assert_eq!(attributes[2].location, 2);
  }

  #[test]
  fn test_parse_glsl_attributes_glsl100() {
    let source_str = r#"
precision highp float;

attribute vec3 position;
attribute vec3 normal;
attribute vec2 texCoord;

varying vec3 vNormal;
varying vec2 vTexCoord;

void main() {
  gl_Position = vec4(position, 1.0);
  vNormal = normal;
  vTexCoord = texCoord;
}
"#;
    let mut parser = GLSLAttributeParser::new();
    parser.parse(source_str).expect("Failed to parse GLSL");

    let attributes = parser.get_attributes();
    assert_eq!(attributes.len(), 3);

    assert_eq!(attributes[0].name, "position");
    assert_eq!(attributes[0].type_name, "vec3");

    assert_eq!(attributes[1].name, "normal");
    assert_eq!(attributes[1].type_name, "vec3");

    assert_eq!(attributes[2].name, "texCoord");
    assert_eq!(attributes[2].type_name, "vec2");
  }

  #[test]
  fn test_get_attrib_location() {
    let source_str = r#"
#version 300 es
in vec3 position;
in vec3 normal;
in vec2 uv;

out vec3 vNormal;
out vec2 vUv;

void main() {
  gl_Position = vec4(position, 1.0);
  vNormal = normal;
  vUv = uv;
}
"#;
    let mut parser = GLSLAttributeParser::new();
    parser.parse(source_str).expect("Failed to parse GLSL");

    assert_eq!(parser.get_attrib_location("position"), Some(0));
    assert_eq!(parser.get_attrib_location("normal"), Some(1));
    assert_eq!(parser.get_attrib_location("uv"), Some(2));
    assert_eq!(parser.get_attrib_location("nonexistent"), None);
  }

  #[test]
  fn test_parse_glsl_attributes_ffi() {
    let source_str = r#"
#version 300 es
in vec3 position;
in vec3 normal;

out vec3 vNormal;

void main() {
  gl_Position = vec4(position, 1.0);
  vNormal = normal;
}
"#;
    let result = parse_glsl_attributes(source_str);
    
    // Parse the JSON result
    let attributes: Vec<GLSLAttribute> = serde_json::from_str(&result).expect("Failed to parse JSON");
    assert_eq!(attributes.len(), 2);
    assert_eq!(attributes[0].name, "position");
    assert_eq!(attributes[1].name, "normal");
  }

  #[test]
  fn test_parse_glsl_attributes_filters_inactive() {
    let source_str = r#"
#version 300 es
in vec3 position;
in vec3 normal;
in vec2 unused_attr;

out vec3 vNormal;

void main() {
  gl_Position = vec4(position, 1.0);
  vNormal = normal;
}
"#;
    let mut parser = GLSLAttributeParser::new();
    parser.parse(source_str).expect("Failed to parse GLSL");
    
    let attributes = parser.get_attributes();
    // Only position and normal should be returned, unused_attr should be filtered out
    assert_eq!(attributes.len(), 2);
    assert_eq!(attributes[0].name, "position");
    assert_eq!(attributes[1].name, "normal");
    
    // unused_attr should not be found
    assert_eq!(parser.get_attrib_location("unused_attr"), None);
  }
}
