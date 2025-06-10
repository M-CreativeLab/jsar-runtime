use std::boxed::Box;
use std::io::Read;
use std::ptr;
use std::slice;

use flate2::read::GzDecoder;
use lazy_static::lazy_static;

mod url_parser;

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

#[no_mangle]
extern "C" fn carbonite_decompress_binary(
  input_ptr: *const u8,
  input_len: usize,
  output_ptr: *mut *mut u8,
  output_len: *mut usize,
) -> usize {
  if input_ptr.is_null() {
    return 1; // Error: invalid input
  }

  let input_slice = unsafe { slice::from_raw_parts(input_ptr, input_len) };
  let mut decoder = GzDecoder::new(input_slice);
  let mut decompressed_data = Vec::new();

  // Decompress the data to the end
  match decoder.read_to_end(&mut decompressed_data) {
    Ok(_) => {
      let decompressed_len = decompressed_data.len();
      let boxed_slice = decompressed_data.into_boxed_slice();
      let raw_ptr = Box::into_raw(boxed_slice) as *mut u8;

      // Update the output pointers.
      unsafe {
        *output_ptr = raw_ptr;
        *output_len = decompressed_len;
      }
      0 // Success
    }
    Err(_) => {
      return 3; // Error: failed to decompress
    }
  }
}

#[no_mangle]
extern "C" fn carbonite_release_memory(ptr: *mut u8, len: usize) {
  if ptr.is_null() {
    return;
  }

  unsafe {
    let _ = Box::from_raw(ptr::slice_from_raw_parts_mut(ptr, len));
  }
}

#[cfg(test)]
mod tests {
  use super::*;

  #[test]
  fn test_decompress_js_source() {
    let js = include_bytes!("jsar-bootstrap-babylon.js.gz");
    let decompressed_js = decompress_js_source(js);
    assert_eq!(decompressed_js.len() > 0, true);
  }

  #[test]
  fn test_decompress_binary() {
    let js = include_bytes!("jsar-bootstrap-babylon.js.gz");
    let mut output_ptr: *mut u8 = ptr::null_mut();
    let mut output_len: usize = 0;
    let result =
      carbonite_decompress_binary(js.as_ptr(), js.len(), &mut output_ptr, &mut output_len);
    assert_eq!(result, 0);
    assert_eq!(output_len, 5024048);
    println!("output: {:?}", output_ptr);

    carbonite_release_memory(output_ptr, output_len);
  }
}
