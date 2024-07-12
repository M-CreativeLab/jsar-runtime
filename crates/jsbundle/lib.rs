use flate2::read::GzDecoder;
use lazy_static::lazy_static;
use std::io::Read;

const JSBUNDLE_SRC_COMPRESSED: &[u8] = include_bytes!("jsbundle.js.gz");
lazy_static! {
  static ref JSBUNDLE_SRC: String = {
    println!("Decompressing the JS bundle...");
    let mut decoder = GzDecoder::new(JSBUNDLE_SRC_COMPRESSED);
    let mut decompressed_js = String::new();
    decoder.read_to_string(&mut decompressed_js).unwrap();
    println!("Decompression has been finished.");
    decompressed_js
  };
}

pub fn get_jsbundle_ptr() -> *const u8 {
  JSBUNDLE_SRC.as_ptr()
}

pub fn get_jsbundle_size() -> usize {
  JSBUNDLE_SRC.len()
}
