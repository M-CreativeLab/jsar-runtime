use std::env;
use std::fs;
use std::io::Error;
use std::path::Path;

extern crate napi_build;

fn main() {
  napi_build::setup();

  let current_dir = env::current_dir().unwrap();
  let file_name = "jsar_jsbindings.h";
  let source_path = current_dir.join(file_name);
  println!("cargo:rerun-if-changed={}", source_path.display());

  if !source_path.exists() {
    panic!("Error: File '{}' not found.", file_name);
  }

  let destination_path = current_dir
    .join("../../build/output/headers/crates/")
    .join(file_name);
  match copy_file(&source_path, &destination_path) {
    Ok(_) => println!(
      "File '{}' copied to '{}'",
      file_name,
      destination_path.to_str().unwrap()
    ),
    Err(e) => panic!("Error copying file: {:?}", e),
  }
}

fn copy_file(source: &Path, destination: &Path) -> Result<(), Error> {
  let content = fs::read_to_string(source)?;
  fs::write(destination, content)?;
  Ok(())
}
