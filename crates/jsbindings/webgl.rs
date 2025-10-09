use glsl_lang::ast;
use glsl_lang::visitor::{Host, HostMut, Visit, Visitor, VisitorMut};
use std::collections::BTreeSet;
use std::path::Path;

use crate::glsl_transpiler;

const FRAGMENT_OUTPUT_NAME: &str = "fragColor";

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
enum ShaderStage {
  Vertex,
  Fragment,
}

#[derive(Default)]
struct StageDetector {
  has_fragment_builtin: bool,
}

impl StageDetector {
  fn stage(&self) -> ShaderStage {
    if self.has_fragment_builtin {
      ShaderStage::Fragment
    } else {
      ShaderStage::Vertex
    }
  }
}

impl Visitor for StageDetector {
  fn visit_identifier(&mut self, ident: &ast::Identifier) -> Visit {
    let name = ident.content.as_str();
    match name {
      "gl_FragColor" | "gl_FragData" | "gl_FragDepth" | "gl_FragCoord" | "gl_SampleMask" => {
        self.has_fragment_builtin = true;
      }
      _ => {}
    }
    Visit::Children
  }
}

struct MyGLSLPatcher {
  stage: ShaderStage,
  saw_gl_fragcolor: bool,
  fragment_output_declared: bool,
  fragdata_indices: BTreeSet<u32>,
}

impl MyGLSLPatcher {
  fn fragment_output_name(index: u32) -> String {
    if index == 0 {
      FRAGMENT_OUTPUT_NAME.to_string()
    } else {
      format!("{}{}", FRAGMENT_OUTPUT_NAME, index)
    }
  }

  fn fragment_output_exists(tu: &ast::TranslationUnit, name: &str) -> bool {
    tu.0.iter().any(|decl| {
      if let ast::ExternalDeclarationData::Declaration(decl_node) = &decl.content {
        if let ast::DeclarationData::InitDeclaratorList(list) = &decl_node.content {
          let head = &list.content.head.content;
          if let Some(identifier) = &head.name {
            return identifier.content.as_str() == name;
          }
        }
      }
      false
    })
  }

  fn fragment_output_insertion_index(tu: &ast::TranslationUnit) -> usize {
    let mut insertion_index = 0;
    for (idx, decl) in tu.0.iter().enumerate() {
      match &decl.content {
        ast::ExternalDeclarationData::Preprocessor(_) => {
          insertion_index = idx + 1;
        }
        ast::ExternalDeclarationData::Declaration(_) => {
          insertion_index = idx + 1;
        }
        ast::ExternalDeclarationData::FunctionDefinition(_) => {
          break;
        }
      }
    }
    insertion_index
  }

  fn extract_constant_u32(expr: &ast::Expr) -> Option<u32> {
    match &expr.content {
      ast::ExprData::IntConst(value) => value.to_string().parse().ok(),
      ast::ExprData::UIntConst(value) => value
        .to_string()
        .trim_end_matches('u')
        .parse()
        .ok(),
      _ => None,
    }
  }

  fn ensure_additional_fragdata_declarations(&mut self, tu: &mut ast::TranslationUnit) {
    let need_fragcolor = self.saw_gl_fragcolor || self.fragdata_indices.contains(&0);
    if need_fragcolor && !self.fragment_output_declared {
      self.ensure_fragment_output_declaration(tu);
    }

    let extra_indices: Vec<u32> = self
      .fragdata_indices
      .iter()
      .copied()
      .filter(|&idx| idx != 0)
      .collect();

    for index in extra_indices {
      let name = Self::fragment_output_name(index);
      if Self::fragment_output_exists(tu, &name) {
        continue;
      }

      let insertion_index = Self::fragment_output_insertion_index(tu);
      tu.0
        .insert(insertion_index, self.build_fragment_output_declaration(&name));
    }
  }

  fn new(stage: ShaderStage) -> Self {
    Self {
      stage,
      saw_gl_fragcolor: false,
      fragment_output_declared: false,
      fragdata_indices: BTreeSet::new(),
    }
  }

  fn apply(&mut self, tu: &mut ast::TranslationUnit) {
    self.remove_precision_declarations(tu);
    tu.visit_mut(self);
    if self.stage == ShaderStage::Fragment {
      if self.saw_gl_fragcolor {
        self.ensure_fragment_output_declaration(tu);
      }
      self.ensure_additional_fragdata_declarations(tu);
    }
    self.ensure_version_and_extension_order(tu);
  }

  fn remove_precision_declarations(&self, tu: &mut ast::TranslationUnit) {
    tu.0.retain(|decl| {
      !matches!(
        &decl.content,
        ast::ExternalDeclarationData::Declaration(node)
          if matches!(node.content, ast::DeclarationData::Precision(_, _))
      )
    });
  }

  fn sanitize_type_qualifier(&self, qualifier: &mut ast::TypeQualifier) {
    use ast::{StorageQualifierData, TypeQualifierSpecData};

    let mut seen_storage: Option<StorageQualifierData> = None;
    let mut sanitized = Vec::with_capacity(qualifier.content.qualifiers.len());

    for mut spec in qualifier.content.qualifiers.drain(..) {
      let keep = match &mut spec.content {
        TypeQualifierSpecData::Storage(storage) => {
          match storage.content {
            StorageQualifierData::Attribute => {
              storage.content = StorageQualifierData::In;
            }
            StorageQualifierData::Varying => {
              storage.content = match self.stage {
                ShaderStage::Vertex => StorageQualifierData::Out,
                ShaderStage::Fragment => StorageQualifierData::In,
              };
            }
            _ => {}
          }

          if let Some(existing) = &seen_storage {
            if existing == &storage.content {
              false
            } else {
              seen_storage = Some(storage.content.clone());
              true
            }
          } else {
            seen_storage = Some(storage.content.clone());
            true
          }
        }
        TypeQualifierSpecData::Precision(_) => false,
        _ => true,
      };

      if keep {
        sanitized.push(spec);
      }
    }

    qualifier.content.qualifiers = sanitized;
  }

  fn create_model_view_matrix_expr(&self) -> ast::Expr {
    let new_lhs: ast::Expr =
      ast::ExprData::Variable(ast::IdentifierData::from("viewMatrix").into()).into();
    let new_rhs: ast::Expr =
      ast::ExprData::Variable(ast::IdentifierData::from("modelMatrix").into()).into();
    ast::ExprData::Binary(
      ast::BinaryOpData::Mult.into(),
      Box::new(new_lhs),
      Box::new(new_rhs),
    )
    .into()
  }

  /// Ensures that the fragment shader has a proper output declaration.
  /// If an existing declaration is found (either "fragColor" or "glFragColor"),
  /// it will be updated to the correct type and name. Otherwise, a new declaration is inserted.
  fn ensure_fragment_output_declaration(&mut self, tu: &mut ast::TranslationUnit) {
    if self.fragment_output_declared {
      return;
    }

    // Try to find and update existing fragment output declaration
    if self.try_update_existing_fragment_output(tu) {
      return;
    }

    // No existing declaration found, create a new one
    self.insert_new_fragment_output_declaration(tu);
  }

  /// Attempts to find and update an existing fragment output declaration.
  /// Returns true if an existing declaration was found and updated.
  fn try_update_existing_fragment_output(&mut self, tu: &mut ast::TranslationUnit) -> bool {
    for decl in &mut tu.0 {
      if let Some(declaration_list) = self.extract_declaration_list(decl) {
        let head = &mut declaration_list.content.head.content;
        
        if let Some(variable_name) = &mut head.name {
          let identifier = variable_name.content.as_str();
          
          if self.is_fragment_output_variable(identifier) {
            // Update the existing declaration
            variable_name.content = ast::IdentifierData::from(FRAGMENT_OUTPUT_NAME);
            self.prepare_fragment_output_type(&mut head.ty);
            declaration_list.content.tail.clear();
            self.fragment_output_declared = true;
            return true;
          }
        }
      }
    }
    false
  }

  /// Extracts the InitDeclaratorList from a declaration if it exists.
  fn extract_declaration_list<'a>(&self, decl: &'a mut ast::ExternalDeclaration) 
    -> Option<&'a mut ast::InitDeclaratorList> {
    if let ast::ExternalDeclarationData::Declaration(decl_node) = &mut decl.content {
      if let ast::DeclarationData::InitDeclaratorList(list) = &mut decl_node.content {
        return Some(list);
      }
    }
    None
  }

  /// Checks if the given identifier represents a fragment output variable.
  fn is_fragment_output_variable(&self, identifier: &str) -> bool {
    identifier == FRAGMENT_OUTPUT_NAME || identifier == "glFragColor"
  }

  /// Inserts a new fragment output declaration at the appropriate position.
  fn insert_new_fragment_output_declaration(&mut self, tu: &mut ast::TranslationUnit) {
    let insertion_index = Self::fragment_output_insertion_index(tu);
    let new_declaration = self.build_fragment_output_declaration(FRAGMENT_OUTPUT_NAME);
    tu.0.insert(insertion_index, new_declaration);
    self.fragment_output_declared = true;
  }

  fn prepare_fragment_output_type(&self, ty: &mut ast::FullySpecifiedType) {
    ty.content.ty = ast::TypeSpecifierData {
      ty: ast::TypeSpecifierNonArrayData::Vec4.into(),
      array_specifier: None,
    }
    .into();

    let qualifier = ty.content.qualifier.get_or_insert_with(|| {
      ast::TypeQualifierData {
        qualifiers: Vec::new(),
      }
      .into()
    });

    self.sanitize_type_qualifier(qualifier);

    let has_out = qualifier.content.qualifiers.iter().any(|spec| {
      matches!(
        &spec.content,
        ast::TypeQualifierSpecData::Storage(storage)
          if matches!(storage.content, ast::StorageQualifierData::Out)
      )
    });

    if !has_out {
      qualifier
        .content
        .qualifiers
        .push(ast::TypeQualifierSpecData::Storage(ast::StorageQualifierData::Out.into()).into());
    }

    if qualifier.content.qualifiers.is_empty() {
      ty.content.qualifier = None;
    }
  }

  fn build_fragment_output_declaration(&self, name: &str) -> ast::ExternalDeclaration {
    let qualifier: ast::TypeQualifier = ast::TypeQualifierData {
      qualifiers: vec![
        ast::TypeQualifierSpecData::Storage(ast::StorageQualifierData::Out.into()).into(),
      ],
    }
    .into();

    let ty: ast::FullySpecifiedType = ast::FullySpecifiedTypeData {
      qualifier: Some(qualifier),
      ty: ast::TypeSpecifierData {
        ty: ast::TypeSpecifierNonArrayData::Vec4.into(),
        array_specifier: None,
      }
      .into(),
    }
    .into();

    let single_decl: ast::SingleDeclaration = ast::SingleDeclarationData {
      ty,
      name: Some(ast::IdentifierData::from(name).into()),
      array_specifier: None,
      initializer: None,
    }
    .into();

    let init_list: ast::InitDeclaratorList = ast::InitDeclaratorListData {
      head: single_decl,
      tail: Vec::new(),
    }
    .into();

    ast::ExternalDeclarationData::Declaration(
      ast::DeclarationData::InitDeclaratorList(init_list).into(),
    )
    .into()
  }

  fn ensure_version_and_extension_order(&self, tu: &mut ast::TranslationUnit) {
    let mut versions = Vec::new();
    let mut extensions = Vec::new();

    tu.0.retain(|decl| {
      if let ast::ExternalDeclarationData::Preprocessor(preprocessor) = &decl.content {
        match &preprocessor.content {
          ast::PreprocessorData::Version(_) => {
            let mut updated = decl.clone();
            if let ast::ExternalDeclarationData::Preprocessor(pre) = &mut updated.content {
              if let ast::PreprocessorData::Version(version) = &mut pre.content {
                version.content.version = 300;
                version.content.profile = Some(ast::PreprocessorVersionProfileData::Es.into());
              }
            }
            versions.push(updated);
            return false;
          }
          ast::PreprocessorData::Extension(_) => {
            extensions.push(decl.clone());
            return false;
          }
          _ => {}
        }
      }
      true
    });

    if versions.is_empty() {
      let version = ast::PreprocessorVersionData {
        version: 300,
        profile: Some(ast::PreprocessorVersionProfileData::Es.into()),
      }
      .into();
      versions.push(
        ast::ExternalDeclarationData::Preprocessor(ast::PreprocessorData::Version(version).into())
          .into(),
      );
    }

    tu.0.splice(0..0, extensions);
    tu.0.splice(0..0, versions);
  }

  fn handle_expr(&mut self, expr: &mut ast::Expr) -> bool {
    match &mut expr.content {
      ast::ExprData::Variable(identifier) => {
        if identifier.content.as_str() == "modelViewMatrix" {
          *expr = self.create_model_view_matrix_expr();
          return true;
        }
        if self.stage == ShaderStage::Fragment && identifier.content.as_str() == "gl_FragColor" {
          identifier.content = ast::IdentifierData::from(FRAGMENT_OUTPUT_NAME);
          self.saw_gl_fragcolor = true;
          return true;
        }
        false
      }
      ast::ExprData::Unary(_, operand) => self.handle_expr(operand),
      ast::ExprData::Binary(_, lhs, rhs) => {
        let l = self.handle_expr(lhs);
        let r = self.handle_expr(rhs);
        l || r
      }
      ast::ExprData::Assignment(lhs, _, rhs) => {
        let l = self.handle_expr(lhs);
        let r = self.handle_expr(rhs);
        l || r
      }
      ast::ExprData::FunCall(_, args) => {
        let mut changed = false;
        for arg in args {
          changed |= self.handle_expr(arg);
        }
        changed
      }
      ast::ExprData::Ternary(cond, then_branch, else_branch) => {
        let r1 = self.handle_expr(cond);
        let r2 = self.handle_expr(then_branch);
        let r3 = self.handle_expr(else_branch);
        r1 || r2 || r3
      }
      ast::ExprData::Comma(lhs, rhs) => self.handle_expr(lhs) || self.handle_expr(rhs),
      ast::ExprData::Bracket(inner, index) => {
        if self.stage == ShaderStage::Fragment {
          if let ast::ExprData::Variable(identifier) = &inner.content {
            if identifier.content.as_str() == "gl_FragData" {
              if let Some(idx) = Self::extract_constant_u32(index) {
                let name = Self::fragment_output_name(idx);
                *expr = ast::ExprData::Variable(ast::IdentifierData::from(name.as_str()).into()).into();
                self.fragdata_indices.insert(idx);
                if idx == 0 {
                  self.saw_gl_fragcolor = true;
                }
                return true;
              }
            }
          }
        }
        self.handle_expr(inner) || self.handle_expr(index)
      }
      ast::ExprData::Dot(inner, _) => self.handle_expr(inner),
      ast::ExprData::PostInc(inner) | ast::ExprData::PostDec(inner) => self.handle_expr(inner),
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

  fn visit_full_specified_type(&mut self, ty: &mut ast::FullySpecifiedType) -> Visit {
    if let Some(qualifier) = &mut ty.content.qualifier {
      self.sanitize_type_qualifier(qualifier);
      if qualifier.content.qualifiers.is_empty() {
        ty.content.qualifier = None;
      }
    }
    Visit::Children
  }

  fn visit_struct_field_specifier(&mut self, field: &mut ast::StructFieldSpecifier) -> Visit {
    if let Some(qualifier) = &mut field.content.qualifier {
      self.sanitize_type_qualifier(qualifier);
      if qualifier.content.qualifiers.is_empty() {
        field.content.qualifier = None;
      }
    }
    Visit::Children
  }

  fn visit_type_qualifier(&mut self, qualifier: &mut ast::TypeQualifier) -> Visit {
    self.sanitize_type_qualifier(qualifier);
    Visit::Children
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

  let mut detector = StageDetector::default();
  tu.visit(&mut detector);
  let stage = detector.stage();
  let mut patcher = MyGLSLPatcher::new(stage);
  patcher.apply(&mut tu);

  let mut s = String::new();
  glsl_transpiler::glsl::show_translation_unit(
    &mut s,
    &tu,
    glsl_transpiler::glsl::FormattingState::default(),
  )
  .expect("Failed to show GLSL");
  s
}

#[cxx::bridge(namespace = "holocron::webgl")]
mod ffi {
  extern "Rust" {
    #[cxx_name = "patchGLSLSourceFromStr"]
    fn patch_glsl_source_from_str(input: &str) -> String;
  }
}

#[cfg(test)]
mod tests {
  use super::*;

  #[test]
  fn test_fragment_shader_rewrites_gl_frag_color() {
    let source_str = r#"
precision mediump float;
varying highp vec2 vUv;

void main() {
  gl_FragColor = vec4(vUv, 0.0, 1.0);
}
"#;
    let patched_source_str = patch_glsl_source_from_str(source_str);
    assert_eq!(
      patched_source_str,
      r#"#version 300 es
in vec2 vUv;
out vec4 fragColor;
void main() {
    fragColor = vec4(vUv, 0., 1.);
}
"#
    )
  }

  #[test]
  fn test_vertex_attribute_and_varying_transforms() {
    let source_str = r#"
attribute vec3 position;
attribute vec3 normal;
varying mediump vec2 vUv;

void main() {
  gl_Position = vec4(position, 1.0);
}
"#;
    let patched_source_str = patch_glsl_source_from_str(source_str);
    assert_eq!(
      patched_source_str,
      r#"#version 300 es
in vec3 position;
in vec3 normal;
out vec2 vUv;
void main() {
    gl_Position = vec4(position, 1.);
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
  fn test_patch_glsl_source_missing_version_webgl1_fragment() {
    let source_str = r#"
precision mediump float;
void main() {
    gl_FragColor = vec4(0., 1., 0., 1.);
}
"#;
    let patched_source_str = patch_glsl_source_from_str(source_str);
    // WebGL 1.0 shaders should remain unchanged (no version injection)
    assert_eq!(
      patched_source_str,
      r#"precision mediump float;
void main() {
    gl_FragColor = vec4(0., 1., 0., 1.);
}
"#
    );
  }

  #[test]
  fn test_patch_glsl_source_missing_version_webgl2_fragment() {
    let source_str = r#"
precision mediump float;
out vec4 fragColor;
void main() {
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
"#;
    let patched_source_str = patch_glsl_source_from_str(source_str);
    // WebGL 2.0 shaders should get #version 300 es injected
    assert_eq!(
      patched_source_str,
      r#"#version 300 es
precision mediump float;
out vec4 fragColor;
void main() {
    fragColor = vec4(1., 0., 0., 1.);
}
"#
    );
  }

  #[test]
  fn test_patch_glsl_source_missing_version_webgl1_vertex() {
    let source_str = r#"
attribute vec4 a_position;
varying vec2 v_texcoord;
void main() {
    gl_Position = a_position;
    v_texcoord = a_position.xy;
}
"#;
    let patched_source_str = patch_glsl_source_from_str(source_str);
    // WebGL 1.0 vertex shader should remain unchanged
    assert_eq!(
      patched_source_str,
      r#"attribute vec4 a_position;
varying vec2 v_texcoord;
void main() {
    gl_Position = a_position;
    v_texcoord = a_position.xy;
}
"#
    );
  }

  #[test]
  fn test_patch_glsl_source_missing_version_webgl2_vertex() {
    let source_str = r#"
layout(location = 0) in vec4 a_position;
out vec2 v_texcoord;
void main() {
    gl_Position = a_position;
    v_texcoord = a_position.xy;
}
"#;
    let patched_source_str = patch_glsl_source_from_str(source_str);
    // WebGL 2.0 vertex shader should get #version 300 es injected
    assert_eq!(
      patched_source_str,
      r#"#version 300 es
layout(location = 0) in vec4 a_position;
out vec2 v_texcoord;
void main() {
    gl_Position = a_position;
    v_texcoord = a_position.xy;
}
"#
    );
  }

  #[test]
  fn test_patch_glsl_source_existing_version_unchanged() {
    let source_str = r#"#version 100
precision mediump float;
void main() {
    gl_FragColor = vec4(0., 1., 0., 1.);
}
"#;
    let patched_source_str = patch_glsl_source_from_str(source_str);
    // Existing version should remain unchanged
    assert_eq!(
      patched_source_str,
      r#"#version 100
precision mediump float;
void main() {
    gl_FragColor = vec4(0., 1., 0., 1.);
}
"#
    );
  }
}
