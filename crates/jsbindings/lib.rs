#![allow(unused_variables)]
#![allow(clippy::uninlined_format_args)]
#![allow(deprecated)]

extern crate ctor;
extern crate jsar_jsbinding_macro;
extern crate log;

mod css_parser;
mod glsl_transpiler;
mod layout;
mod typescript_transpiler;

use std::ffi::CString;
use std::os::raw::c_char;
use std::path::Path;
use url::Url;

use glsl_lang::ast;
use glsl_lang::visitor::{HostMut, Visit, VisitorMut};

#[no_mangle]
extern "C" fn release_rust_cstring(s: *mut c_char) {
  unsafe {
    if s.is_null() {
      return;
    }
    let _ = CString::from_raw(s);
  }
}

#[no_mangle]
extern "C" fn release_rust_cstrings(list: *mut *mut c_char) {
  if list.is_null() {
    return;
  }
  unsafe {
    let mut i = 0;
    while !(*list.offset(i)).is_null() {
      let _ = CString::from_raw(*list.offset(i));
      i += 1;
    }
    let len: usize = i.try_into().unwrap();
    Vec::from_raw_parts(list, len, len);
  }
}

#[repr(C)]
pub struct WHATWGUrl {
  host: *mut c_char,
  hostname: *mut c_char,
  href: *mut c_char,
  origin: *mut c_char,
  password: *mut c_char,
  pathname: *mut c_char,
  port: i32,
  protocol: *mut c_char,
  search: *mut c_char,
  username: *mut c_char,
  hash: *mut c_char,
}

#[no_mangle]
extern "C" fn parse_whatwg_url(input: *const c_char) -> WHATWGUrl {
  let input_str: &str = unsafe { std::ffi::CStr::from_ptr(input) }
    .to_str()
    .expect("Failed to convert C string to Rust string");
  let url = Url::parse(input_str).expect("Failed to parse URL");
  let hostname = url.host_str().unwrap_or("");
  let port = url.port().unwrap_or(0);
  let host = format!("{}:{}", hostname, port);
  let origin = url.origin().ascii_serialization();
  let protocol = format!("{}:", url.scheme());

  WHATWGUrl {
    host: CString::new(host)
      .expect("Failed to create host CString")
      .into_raw(),
    hostname: CString::new(hostname)
      .expect("Failed to create hostname CString")
      .into_raw(),
    port: port.into(),
    href: CString::new(url.as_str())
      .expect("Failed to create href CString")
      .into_raw(),
    origin: CString::new(origin)
      .expect("Failed to create origin CString")
      .into_raw(),
    password: CString::new(url.password().unwrap_or(""))
      .expect("Failed to create password CString")
      .into_raw(),
    pathname: CString::new(url.path())
      .expect("Failed to create pathname CString")
      .into_raw(),
    protocol: CString::new(protocol)
      .expect("Failed to create protocol CString")
      .into_raw(),
    search: CString::new(url.query().unwrap_or(""))
      .expect("Failed to create search CString")
      .into_raw(),
    username: CString::new(url.username())
      .expect("Failed to create username CString")
      .into_raw(),
    hash: CString::new(url.fragment().unwrap_or(""))
      .expect("Failed to create hash CString")
      .into_raw(),
  }
}

#[no_mangle]
extern "C" fn release_whatwg_url(url: WHATWGUrl) {
  unsafe {
    if !url.host.is_null() {
      let _ = CString::from_raw(url.host);
    }
    if !url.hostname.is_null() {
      let _ = CString::from_raw(url.hostname);
    }
    if !url.href.is_null() {
      let _ = CString::from_raw(url.href);
    }
    if !url.origin.is_null() {
      let _ = CString::from_raw(url.origin);
    }
    if !url.password.is_null() {
      let _ = CString::from_raw(url.password);
    }
    if !url.pathname.is_null() {
      let _ = CString::from_raw(url.pathname);
    }
    if !url.protocol.is_null() {
      let _ = CString::from_raw(url.protocol);
    }
    if !url.search.is_null() {
      let _ = CString::from_raw(url.search);
    }
    if !url.username.is_null() {
      let _ = CString::from_raw(url.username);
    }
    if !url.hash.is_null() {
      let _ = CString::from_raw(url.hash);
    }
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

#[derive(PartialEq)]
#[repr(i32)]
enum ModuleExtensionIndex {
  None = 0,
  JavaScript,
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

impl std::fmt::Debug for ModuleExtensionIndex {
  fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
    match self {
      ModuleExtensionIndex::None => write!(f, "None"),
      ModuleExtensionIndex::JavaScript => write!(f, "JavaScript"),
      ModuleExtensionIndex::TypeScript => write!(f, "TypeScript"),
      ModuleExtensionIndex::JSON => write!(f, "JSON"),
      ModuleExtensionIndex::Bin => write!(f, "Bin"),
      ModuleExtensionIndex::Data => write!(f, "Data"),
      ModuleExtensionIndex::WebAssembly => write!(f, "WebAssembly"),
      ModuleExtensionIndex::PNG => write!(f, "PNG"),
      ModuleExtensionIndex::JPEG => write!(f, "JPEG"),
      ModuleExtensionIndex::GIF => write!(f, "GIF"),
      ModuleExtensionIndex::SVG => write!(f, "SVG"),
      ModuleExtensionIndex::MP3 => write!(f, "MP3"),
      ModuleExtensionIndex::WAV => write!(f, "WAV"),
      ModuleExtensionIndex::OGG => write!(f, "OGG"),
    }
  }
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
      _ => ModuleExtensionIndex::None,
    }
  } else {
    ModuleExtensionIndex::None
  }
}

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

#[no_mangle]
extern "C" fn patch_glsl_source(source_str: *const c_char) -> *mut c_char {
  let source_string: &str = unsafe { std::ffi::CStr::from_ptr(source_str) }
    .to_str()
    .expect("Failed to read GLSL source string");
  let patched_source_str = CString::new(patch_glsl_source_from_str(source_string))
    .expect("Failed to create GLSL source CString");
  patched_source_str.into_raw()
}

#[repr(C)]
pub struct TranspiledTypeScriptOutput {
  code: *mut c_char,
  error_message: *mut c_char,
}

#[no_mangle]
extern "C" fn transpile_typescript_to_js(input_str: *const c_char) -> TranspiledTypeScriptOutput {
  let input_string: &str = unsafe { std::ffi::CStr::from_ptr(input_str) }
    .to_str()
    .expect("Failed to read TypeScript input string");

  let output = typescript_transpiler::transpile_typescript_to_js(input_string.into());
  if let Err(e) = output {
    let error_message = format!("Error: {:?}", e);
    let error_message_cstring =
      CString::new(error_message).expect("Failed to create error CString");
    TranspiledTypeScriptOutput {
      code: std::ptr::null_mut(),
      error_message: error_message_cstring.into_raw(),
    }
  } else {
    let output_string = output.unwrap().code;
    let output_cstring = CString::new(output_string).expect("Failed to create output CString");
    TranspiledTypeScriptOutput {
      code: output_cstring.into_raw(),
      error_message: std::ptr::null_mut(),
    }
  }
}

#[no_mangle]
extern "C" fn release_transpiled_typescript_output(output: TranspiledTypeScriptOutput) {
  unsafe {
    if !output.code.is_null() {
      let _ = CString::from_raw(output.code);
    }
    if !output.error_message.is_null() {
      let _ = CString::from_raw(output.error_message);
    }
  }
}

#[cfg(test)]
mod tests {
  use super::*;
  use std::ffi::CString;

  #[test]
  fn test_parse_whatwg_url() {
    let url_str = CString::new("https://example.com:8080/path?query#fragment").unwrap();
    let url = parse_whatwg_url(url_str.as_ptr());
    assert_eq!(
      unsafe { CString::from_raw(url.host).to_str().unwrap() },
      "example.com:8080"
    );
    assert_eq!(
      unsafe { CString::from_raw(url.hostname).to_str().unwrap() },
      "example.com"
    );
    assert_eq!(url.port, 8080);
    assert_eq!(
      unsafe { CString::from_raw(url.href).to_str().unwrap() },
      "https://example.com:8080/path?query#fragment"
    );
    assert_eq!(
      unsafe { CString::from_raw(url.origin).to_str().unwrap() },
      "https://example.com:8080"
    );
    assert_eq!(
      unsafe { CString::from_raw(url.password).to_str().unwrap() },
      ""
    );
    assert_eq!(
      unsafe { CString::from_raw(url.pathname).to_str().unwrap() },
      "/path"
    );
    assert_eq!(
      unsafe { CString::from_raw(url.protocol).to_str().unwrap() },
      "https:"
    );
    assert_eq!(
      unsafe { CString::from_raw(url.search).to_str().unwrap() },
      "query"
    );
    assert_eq!(
      unsafe { CString::from_raw(url.username).to_str().unwrap() },
      ""
    );
    assert_eq!(
      unsafe { CString::from_raw(url.hash).to_str().unwrap() },
      "fragment"
    );
  }

  #[test]
  fn test_parse_url_to_module_extension() {
    let url_str = CString::new("https://example.com/index.js").unwrap();
    let extension = parse_url_to_module_extension(url_str.as_ptr());
    assert_eq!(extension, ModuleExtensionIndex::JavaScript);

    let url_str = CString::new("https://example.com/index.ts").unwrap();
    let extension = parse_url_to_module_extension(url_str.as_ptr());
    assert_eq!(extension, ModuleExtensionIndex::TypeScript);

    let url_str = CString::new("https://example.com/data.json").unwrap();
    let extension = parse_url_to_module_extension(url_str.as_ptr());
    assert_eq!(extension, ModuleExtensionIndex::JSON);

    let url_str = CString::new("https://example.com/file.bin").unwrap();
    let extension = parse_url_to_module_extension(url_str.as_ptr());
    assert_eq!(extension, ModuleExtensionIndex::Bin);

    let url_str = CString::new("https://example.com/file.data").unwrap();
    let extension = parse_url_to_module_extension(url_str.as_ptr());
    assert_eq!(extension, ModuleExtensionIndex::Data);

    let url_str = CString::new("https://example.com/file.wasm").unwrap();
    let extension = parse_url_to_module_extension(url_str.as_ptr());
    assert_eq!(extension, ModuleExtensionIndex::WebAssembly);

    let url_str = CString::new("https://example.com/image.png").unwrap();
    let extension = parse_url_to_module_extension(url_str.as_ptr());
    assert_eq!(extension, ModuleExtensionIndex::PNG);

    let url_str = CString::new("https://example.com/image.jpg").unwrap();
    let extension = parse_url_to_module_extension(url_str.as_ptr());
    assert_eq!(extension, ModuleExtensionIndex::JPEG);

    let url_str = CString::new("https://example.com/image.gif").unwrap();
    let extension = parse_url_to_module_extension(url_str.as_ptr());
    assert_eq!(extension, ModuleExtensionIndex::GIF);

    let url_str = CString::new("https://example.com/image.svg").unwrap();
    let extension = parse_url_to_module_extension(url_str.as_ptr());
    assert_eq!(extension, ModuleExtensionIndex::SVG);

    let url_str = CString::new("https://example.com/audio.mp3").unwrap();
    let extension = parse_url_to_module_extension(url_str.as_ptr());
    assert_eq!(extension, ModuleExtensionIndex::MP3);

    let url_str = CString::new("https://example.com/audio.wav").unwrap();
    let extension = parse_url_to_module_extension(url_str.as_ptr());
    assert_eq!(extension, ModuleExtensionIndex::WAV);

    let url_str = CString::new("https://example.com/audio.ogg").unwrap();
    let extension = parse_url_to_module_extension(url_str.as_ptr());
    assert_eq!(extension, ModuleExtensionIndex::OGG);

    let url_str = CString::new("https://example.com/unknown").unwrap();
    let extension = parse_url_to_module_extension(url_str.as_ptr());
    assert_eq!(extension, ModuleExtensionIndex::None);
  }

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
