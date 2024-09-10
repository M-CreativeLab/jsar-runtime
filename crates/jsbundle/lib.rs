use flate2::read::GzDecoder;
use lazy_static::lazy_static;
use std::io::Read;

const JSBOOTSTRAP_BABYLON_COMPRESSED: &[u8] = include_bytes!("jsar-bootstrap-babylon.js.gz");
const JSBUNDLE_CLIENT_ENTRY_COMPRESSED: &[u8] = include_bytes!("jsar-client-entry.js.gz");

fn decompress_js_source(js: &[u8]) -> String {
  let mut decoder = GzDecoder::new(js);
  let mut decompressed_js = String::new();
  decoder.read_to_string(&mut decompressed_js).unwrap();
  decompressed_js
}

lazy_static! {
  static ref JSBOOTSTRAP_BABYLON_SRC: String = decompress_js_source(JSBOOTSTRAP_BABYLON_COMPRESSED);
  static ref JSBUNDLE_CLIENT_ENTRY_SRC: String = decompress_js_source(JSBUNDLE_CLIENT_ENTRY_COMPRESSED);
}

#[no_mangle]
extern "C" fn get_jsbootstrap_ptr(jsframework: i32) -> *const u8 {
  JSBOOTSTRAP_BABYLON_SRC.as_ptr()
}

#[no_mangle]
extern "C" fn get_jsbootstrap_size(jsframework: i32) -> usize {
  JSBOOTSTRAP_BABYLON_SRC.len()
}

#[no_mangle]
extern "C" fn get_jsbundle_ptr() -> *const u8 {
  JSBUNDLE_CLIENT_ENTRY_SRC.as_ptr()
}

#[no_mangle]
extern "C" fn get_jsbundle_size() -> usize {
  JSBUNDLE_CLIENT_ENTRY_SRC.len()
}
