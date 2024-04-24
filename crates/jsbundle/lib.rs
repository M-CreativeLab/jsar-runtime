const JSBUNDLE_SRC: &str = include_str!("jsbundle.js");

pub fn get_jsbundle_ptr() -> *const u8 {
  JSBUNDLE_SRC.as_ptr()
}

pub fn get_jsbundle_size() -> usize {
  JSBUNDLE_SRC.len()
}
