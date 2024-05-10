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
use std::os::raw::{c_char, c_void};

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
