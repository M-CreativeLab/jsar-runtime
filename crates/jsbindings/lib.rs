#![allow(unused_variables)]
#![allow(clippy::uninlined_format_args)]
#![allow(deprecated)]

#[macro_use]
extern crate log;
#[cfg(target_os = "android")]
extern crate android_logger;
extern crate ctor;

#[macro_use]
extern crate jsar_jsbinding_macro;
extern crate jsar_jsbundle;
#[macro_use]
extern crate napi_derive;

pub mod htmlrender;

use cssparser::{Parser, ParserInput};
use std::os::raw::{c_char, c_void};
use style::context::QuirksMode;
use style::parser::ParserContext;
use style::stylesheets::{CssRuleType, Origin};
use style::values::specified::color::Color;
use style_traits::ParsingMode;
use url::Url;

extern "C" {
  #[cfg(target_os = "android")]
  fn eglGetProcAddress(procname: *const c_char) -> *const c_void;
}

#[ctor::ctor]
#[cfg(target_os = "android")]
fn init_platform_env() {
  android_logger::init_once(
    android_logger::Config::default()
      .with_max_level(log::LevelFilter::Trace)
      .with_tag("jsar"),
  );
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

/**
 * Expose the JS bundle to the runtime.
 */
#[no_mangle]
extern "C" fn get_jsbundle_ptr() -> *const u8 {
  jsar_jsbundle::get_jsbundle_ptr()
}

#[no_mangle]
extern "C" fn get_jsbundle_size() -> usize {
  jsar_jsbundle::get_jsbundle_size()
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
