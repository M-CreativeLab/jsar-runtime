use glsl_lang::ast;
use glsl_lang::visitor::{HostMut, Visit, VisitorMut};
use std::ffi::CString;
use std::os::raw::c_char;
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
    ast::TranslationUnit, lexer::v2_full::fs::PreprocessorExt, parse::IntoParseBuilderExt,
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
  use std::ffi::CString;

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
}
