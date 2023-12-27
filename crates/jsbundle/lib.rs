const JSBUNDLE_SRC: &str = include_str!("jsbundle.js");

#[no_mangle]
pub extern "C" fn get_jsbundle() -> *const u8 {
  JSBUNDLE_SRC.as_ptr()
}
