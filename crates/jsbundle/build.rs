use std::env;
use std::fs;
use std::io::Error;
use std::path::Path;

fn main() {
  let current_dir = env::current_dir().unwrap();
  let file_name = "jsar-runtime.js";

  let source_path = current_dir.join("../../build/output").join(file_name);
  if !source_path.exists() {
    panic!(
      "Error: File '{}' not found in directory '{}'",
      file_name,
      source_path.to_str().unwrap()
    );
  }

  let destination_path = current_dir.join("jsbundle.js");
  match copy_file(&source_path, &destination_path) {
    Ok(_) => println!(
      "File '{}' copied to '{}'",
      file_name,
      destination_path.to_str().unwrap()
    ),
    Err(e) => panic!("Error copying file: {:?}", e),
  }

  // use bindgen to generate C/C++ binding and headers
  cbindgen::generate(current_dir.as_path())
    .unwrap_or_else(|err| panic!("Unable to generate C bindings: {}", err))
    .write_to_file(current_dir.join("../../build/output/headers/crates/jsar_jsbundle.h"));
}

fn copy_file(source: &Path, destination: &Path) -> Result<(), Error> {
  let content = fs::read_to_string(source)?;
  fs::write(destination, content)?;
  Ok(())
}
