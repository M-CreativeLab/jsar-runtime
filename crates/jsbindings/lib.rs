#![allow(unused_variables)]
#![allow(clippy::uninlined_format_args)]
#![allow(deprecated)]

#[macro_use]
extern crate log;
extern crate ctor;

#[macro_use]
extern crate jsar_jsbinding_macro;

use cssparser::{Parser, ParserInput};
use std::ffi::CString;
use std::os::raw::{c_char, c_void};
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
      std::ptr::copy_nonoverlapping(new_url_str.as_ptr(), out_url_str as *mut c_char, new_url_len);
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
