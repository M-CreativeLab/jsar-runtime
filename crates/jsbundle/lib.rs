use flate2::read::GzDecoder;
use lazy_static::lazy_static;
use std::io::Read;

#[cfg(all(target_os = "macos", target_arch = "aarch64"))]
mod platform {
  pub const LIBNODE_SRC: &[u8] = include_bytes!("res/aarch64-apple-darwin/libnode.lib");
  pub const LIBNODE_SRC_MD5: &str = include_str!("res/aarch64-apple-darwin/libnode.lib.md5");
}

#[cfg(all(target_os = "macos", target_arch = "x86_64"))]
mod platform {
  pub const LIBNODE_SRC: &[u8] = include_bytes!("res/x86_64-apple-darwin/libnode.lib");
  pub const LIBNODE_SRC_MD5: &str = include_str!("res/x86_64-apple-darwin/libnode.lib.md5");
}

#[cfg(all(target_os = "android", target_arch = "aarch64"))]
mod platform {
  pub const LIBNODE_SRC: &[u8] = include_bytes!("res/aarch64-linux-android/libnode.lib");
  pub const LIBNODE_SRC_MD5: &str = include_str!("res/aarch64-linux-android/libnode.lib.md5");
}

#[cfg(all(target_os = "windows", target_arch = "x86_64"))]
mod platform {
  pub const LIBNODE_SRC: &[u8] = include_bytes!("res/x86_64-windows/libnode.lib");
  pub const LIBNODE_SRC_MD5: &str = include_str!("res/x86_64-windows/libnode.lib.md5");
}

#[cfg(not(any(
  all(target_os = "macos", target_arch = "aarch64"),
  all(target_os = "macos", target_arch = "x86_64"),
  all(target_os = "android"),
  all(target_os = "windows"),
)))]
mod platform {
  pub const LIBNODE_SRC: &[u8] = &[];
  pub const LIBNODE_SRC_MD5: &str = "";
}

const JSBOOTSTRAP_BABYLON_COMPRESSED: &[u8] = include_bytes!("jsar-bootstrap-babylon.js.gz");
const JSBUNDLE_CLIENT_ENTRY_COMPRESSED: &[u8] = include_bytes!("jsar-client-entry.js.gz");
const JSBUNDLE_WEBWORKERS_ENTRY_COMPRESSED: &[u8] = include_bytes!("jsar-webworkers-entry.js.gz");

fn decompress_js_source(js: &[u8]) -> String {
  let mut decoder = GzDecoder::new(js);
  let mut decompressed_js = String::new();
  decoder.read_to_string(&mut decompressed_js).unwrap();
  decompressed_js
}

lazy_static! {
  static ref JSBOOTSTRAP_BABYLON_SRC: String = decompress_js_source(JSBOOTSTRAP_BABYLON_COMPRESSED);
  static ref JSBUNDLE_CLIENT_ENTRY_SRC: String =
    decompress_js_source(JSBUNDLE_CLIENT_ENTRY_COMPRESSED);
  static ref JSBUNDLE_WEBWORKERS_ENTRY_SRC: String =
    decompress_js_source(JSBUNDLE_WEBWORKERS_ENTRY_COMPRESSED);
}

#[no_mangle]
extern "C" fn get_libnode_ptr() -> *const u8 {
  platform::LIBNODE_SRC.as_ptr()
}

#[no_mangle]
extern "C" fn get_libnode_size() -> usize {
  platform::LIBNODE_SRC.len()
}

#[no_mangle]
extern "C" fn get_libnode_md5_ptr() -> *const u8 {
  platform::LIBNODE_SRC_MD5.as_ptr()
}

#[no_mangle]
extern "C" fn get_libnode_md5_size() -> usize {
  platform::LIBNODE_SRC_MD5.len()
}

#[no_mangle]
extern "C" fn get_jsbootstrap_ptr(_framework_id: i32) -> *const u8 {
  JSBOOTSTRAP_BABYLON_SRC.as_ptr()
}

#[no_mangle]
extern "C" fn get_jsbootstrap_size(_framework_id: i32) -> usize {
  JSBOOTSTRAP_BABYLON_SRC.len()
}

#[no_mangle]
extern "C" fn get_jsbundle_ptr(id: i32) -> *const u8 {
  if id == 0 {
    JSBUNDLE_CLIENT_ENTRY_SRC.as_ptr()
  } else if id == 1 {
    JSBUNDLE_WEBWORKERS_ENTRY_SRC.as_ptr()
  } else {
    unreachable!()
  }
}

#[no_mangle]
extern "C" fn get_jsbundle_size(id: i32) -> usize {
  if id == 0 {
    JSBUNDLE_CLIENT_ENTRY_SRC.len()
  } else if id == 1 {
    JSBUNDLE_WEBWORKERS_ENTRY_SRC.len()
  } else {
    unreachable!()
  }
}
