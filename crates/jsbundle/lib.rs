use flate2::read::GzDecoder;
use lazy_static::lazy_static;
use std::io::Read;

const LIBNODE_SRC: &[u8] = include_bytes!("res/libnode.lib");
const LIBNODE_SRC_MD5: &str = include_str!("res/libnode.lib.md5");
const JSBOOTSTRAP_SRC_COMPRESSED: &[u8] = include_bytes!("jsar-bootstrap.js.gz");
const JSBUNDLE_SRC_COMPRESSED: &[u8] = include_bytes!("jsar-runtime.js.gz");

lazy_static! {
  static ref JSBOOTSTRAP_SRC: String = {
    let mut decoder = GzDecoder::new(JSBOOTSTRAP_SRC_COMPRESSED);
    let mut decompressed_js = String::new();
    decoder.read_to_string(&mut decompressed_js).unwrap();
    decompressed_js
  };
  static ref JSBUNDLE_SRC: String = {
    let mut decoder = GzDecoder::new(JSBUNDLE_SRC_COMPRESSED);
    let mut decompressed_js = String::new();
    decoder.read_to_string(&mut decompressed_js).unwrap();
    decompressed_js
  };
}

#[no_mangle]
extern "C" fn get_libnode_ptr() -> *const u8 {
  LIBNODE_SRC.as_ptr()
}

#[no_mangle]
extern "C" fn get_libnode_size() -> usize {
  LIBNODE_SRC.len()
}

#[no_mangle]
extern "C" fn get_libnode_md5_ptr() -> *const u8 {
  LIBNODE_SRC_MD5.as_ptr()
}

#[no_mangle]
extern "C" fn get_libnode_md5_size() -> usize {
  LIBNODE_SRC_MD5.len()
}

#[no_mangle]
extern "C" fn get_jsbootstrap_ptr() -> *const u8 {
  JSBOOTSTRAP_SRC.as_ptr()
}

#[no_mangle]
extern "C" fn get_jsbootstrap_size() -> usize {
  JSBOOTSTRAP_SRC.len()
}

#[no_mangle]
extern "C" fn get_jsbundle_ptr() -> *const u8 {
  JSBUNDLE_SRC.as_ptr()
}

#[no_mangle]
extern "C" fn get_jsbundle_size() -> usize {
  JSBUNDLE_SRC.len()
}
