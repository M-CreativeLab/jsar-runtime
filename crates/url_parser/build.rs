use std::env;
use std::fs;
use std::io::Error;
use std::path::Path;
use std::path::PathBuf;

extern crate env_logger;
extern crate log;

fn main() {
  // Tell cargo to re-run this script if the env FORCE_REBUILD is set.
  // println!("cargo:rerun-if-env-changed=FORCE_REBUILD");
  // Tell cargo to re-run this script if the Rust files have changed.
  println!("cargo:rerun-if-changed=lib.rs");

  env_logger::init();
  generate_module("lib.rs", "url_parser");
}

fn generate_module(source: &str, name: &str) {
  cxx_build::bridge(source).std("c++17").compile(name);

  let out_dir = env::var("OUT_DIR").unwrap();
  let out_path = PathBuf::from(out_dir);
  let out_name = format!("{}.hpp", name);
  let header = out_path.join(format!("cxxbridge/include/jsar_url_parser/{}.h", source));
  let _ = std::fs::copy(header, &out_name).unwrap();
  install_header(&out_name);
}

fn install_header(file_name: &str) {
  let current_dir = env::current_dir().unwrap();
  let source_path = current_dir.join(file_name);
  if !source_path.exists() {
    panic!("Error: File '{}' not found.", file_name);
  }
  let destination_dir = PathBuf::from(&current_dir).join("../../build/output/headers/crates/");
  if !destination_dir.exists() {
    std::fs::create_dir_all(&destination_dir).expect("Failed to create the destination directory.");
  }

  let destination_path = destination_dir.join(file_name);
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
