#![allow(unused_variables)]
#![allow(clippy::uninlined_format_args)]
#![allow(deprecated)]

#[macro_use]
extern crate log;
extern crate ctor;

#[macro_use]
extern crate jsar_jsbinding_macro;

use cssparser::{Parser, ParserInput};
use std::any::Any;
use std::ffi::CString;
use std::os::raw::{c_char, c_void};
use std::path::Path;
use style::context::QuirksMode;
use style::font_face::Source;
use style::media_queries::{Device, MediaType};
use style::parser::{Parse, ParserContext};
use style::properties::{
  parse_one_declaration_into, PropertyDeclarationBlock, PropertyId, ShorthandId,
  SourcePropertyDeclaration,
};
use style::stylesheets::{CssRuleType, Origin};
use style::values::specified::color::Color;
use style_traits::ParsingMode;
use url::Url;

extern "C" {
  #[cfg(target_os = "android")]
  fn eglGetProcAddress(procname: *const c_char) -> *const c_void;
}

#[no_mangle]
pub extern "C" fn jsar_load_gl() {
  // Load the OpenGL function pointers for the rust environment in native thread.
  #[cfg(target_os = "android")]
  gl::load_with(|symbol| unsafe {
    let symbol_cstr = std::ffi::CString::new(symbol).expect("Failed to convert string to C string");
    eglGetProcAddress(symbol_cstr.as_ptr()) as *const _
  });
}

#[repr(C)]
pub struct RGBAColor {
  r: u32,
  g: u32,
  b: u32,
  a: u32,
}

impl RGBAColor {
  fn new(r: u32, g: u32, b: u32, a: u32) -> Self {
    Self { r, g, b, a }
  }
}

#[no_mangle]
extern "C" fn parse_csscolor(color_str: *const c_char) -> RGBAColor {
  let color_string: &str = unsafe { std::ffi::CStr::from_ptr(color_str) }
    .to_str()
    .expect("Failed to convert C string to Rust string");
  let mut input = ParserInput::new(color_string);
  let mut parser = Parser::new(&mut input);
  let url = Url::parse("about:blank").unwrap().into();
  let context = ParserContext::new(
    Origin::Author,
    &url,
    Some(CssRuleType::Style),
    ParsingMode::DEFAULT,
    QuirksMode::NoQuirks,
    Default::default(),
    None,
    None,
  );
  match Color::parse_and_compute(&context, &mut parser, None) {
    Some(color) => {
      let rgba = color.as_absolute().unwrap().into_srgb_legacy();
      RGBAColor::new(
        (rgba.components.0 * 255.0) as u32,
        (rgba.components.1 * 255.0) as u32,
        (rgba.components.2 * 255.0) as u32,
        (rgba.alpha * 255.0) as u32,
      )
    }
    None => RGBAColor::new(0, 0, 0, 1),
  }
}

#[no_mangle]
extern "C" fn create_url_with_path(
  url_str: *const c_char,
  sub_path: *const c_char,
  out_url_str: *mut *mut c_char,
  out_url_max_len: usize,
) -> usize {
  let url_string: &str = unsafe { std::ffi::CStr::from_ptr(url_str) }
    .to_str()
    .expect("Failed to convert C string to Rust string");
  let sub_path_string: &str = unsafe { std::ffi::CStr::from_ptr(sub_path) }
    .to_str()
    .expect("Failed to convert C string to Rust string");

  let url = Url::parse(url_string).unwrap();
  let new_url = url.join(sub_path_string).unwrap();
  let new_url_str = CString::new(new_url.to_string()).expect("Failed to create URL CString");

  let new_url_len = new_url_str.as_bytes().len();
  if new_url_len > out_url_max_len {
    0
  } else {
    // Copy the new URL string to the output buffer.
    unsafe {
      std::ptr::copy_nonoverlapping(
        new_url_str.as_ptr(),
        out_url_str as *mut c_char,
        new_url_len,
      );
    }
    new_url_len
  }
}

#[repr(i32)]
enum ModuleExtensionIndex {
  JavaScript = 0,
  TypeScript,
  JSON,
  Bin,
  Data,
  WebAssembly,
  PNG,
  JPEG,
  GIF,
  SVG,
  MP3,
  WAV,
  OGG,
}

#[no_mangle]
extern "C" fn parse_url_to_module_extension(url_str: *const c_char) -> ModuleExtensionIndex {
  let url_string: &str = unsafe { std::ffi::CStr::from_ptr(url_str) }
    .to_str()
    .expect("Failed to convert C string to Rust string");

  let url = Url::parse(url_string).unwrap();

  // Check if the URL path ends with a file extension like .html, .css, etc.
  if let Some(file_extension) = url.path().rsplit('.').next() {
    match file_extension {
      "js" | "mjs" => ModuleExtensionIndex::JavaScript,
      "ts" => ModuleExtensionIndex::TypeScript,
      "json" => ModuleExtensionIndex::JSON,
      "bin" => ModuleExtensionIndex::Bin,
      "data" => ModuleExtensionIndex::Data,
      "wasm" => ModuleExtensionIndex::WebAssembly,
      "png" => ModuleExtensionIndex::PNG,
      "jpg" | "jpeg" => ModuleExtensionIndex::JPEG,
      "gif" => ModuleExtensionIndex::GIF,
      "svg" => ModuleExtensionIndex::SVG,
      "mp3" => ModuleExtensionIndex::MP3,
      "wav" => ModuleExtensionIndex::WAV,
      "ogg" => ModuleExtensionIndex::OGG,
      _ => ModuleExtensionIndex::JavaScript,
    }
  } else {
    ModuleExtensionIndex::JavaScript
  }
}

mod glsl_transpiler;
use glsl_lang::ast;
use glsl_lang::visitor::{HostMut, Visit, VisitorMut};

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
    ast::{NodeDisplay, TranslationUnit},
    lexer::v2_full::fs::PreprocessorExt,
    parse::IntoParseBuilderExt,
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
    .expect("Failed to parse GLSL");

  let mut my_glsl_patcher = MyGLSLPatcher {};
  tu.visit_mut(&mut my_glsl_patcher);

  let mut s = String::new();
  glsl_transpiler::glsl::show_translation_unit(
    &mut s,
    &tu,
    glsl_transpiler::glsl::FormattingState::default(),
  )
  .expect("Failed to show GLSL");
  s
}

#[no_mangle]
extern "C" fn patch_glsl_source(source_str: *const c_char) -> *mut c_char {
  let source_string: &str = unsafe { std::ffi::CStr::from_ptr(source_str) }
    .to_str()
    .expect("Failed to read GLSL source string");
  let patched_source_str = CString::new(patch_glsl_source_from_str(source_string))
    .expect("Failed to create GLSL source CString");
  patched_source_str.into_raw()
}

#[no_mangle]
extern "C" fn release_rust_cstring(s: *mut c_char) {
  unsafe {
    if s.is_null() {
      return;
    }
    let _ = CString::from_raw(s);
  }
}

#[test]
fn test_patch_glsl_source() {
  let source_str = r#"
precision highp float;
highp float a = 1.0;

void main() { 
  gl_FragColor = vec4(1, 1, 1, 1); 
}"#;
  let patched_source_str = patch_glsl_source_from_str(source_str);
  println!("{}", patched_source_str);
}
