use std::env;
use std::io::Error;
use std::path::Path;

fn main() {
  let current_dir = env::current_dir().unwrap();
  // use bindgen to generate C/C++ binding and headers
  cbindgen::generate(current_dir.as_path())
    .unwrap_or_else(|err| panic!("Unable to generate C bindings: {}", err))
    .write_to_file(current_dir.join("../../build/output/headers/crates/jsar_shaders.h"));
}
