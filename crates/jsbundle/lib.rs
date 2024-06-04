use flate2::read::GzDecoder;
use std::io::Read;

const JSBUNDLE_SRC_COMPRESSED: &[u8] = include_bytes!("jsbundle.js.gz");
static mut JSBUNDLE_SRC: Option<String> = None;

pub fn get_jsbundle_ptr() -> *const u8 {
  decompress_jsbundle();
  unsafe {
    if JSBUNDLE_SRC.is_none() {
      panic!("Failed to decompress JS bundle");
    } else {
      JSBUNDLE_SRC.as_ref().unwrap().as_ptr()
    }
  }
}

pub fn get_jsbundle_size() -> usize {
  decompress_jsbundle();
  unsafe {
    if JSBUNDLE_SRC.is_none() {
      panic!("Failed to decompress JS bundle");
    } else {
      JSBUNDLE_SRC.as_ref().unwrap().len()
    }
  }
}

fn decompress_jsbundle() {
  unsafe {
    if JSBUNDLE_SRC.is_none() {
      let mut decoder = GzDecoder::new(JSBUNDLE_SRC_COMPRESSED);
      let mut decompressed_js = String::new();
      decoder
        .read_to_string(&mut decompressed_js)
        .expect("Failed to decompress JS bundle");
      JSBUNDLE_SRC = Some(decompressed_js);
    }
  }
}
