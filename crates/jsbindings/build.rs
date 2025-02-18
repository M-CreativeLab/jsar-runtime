use std::env;
use std::fs;
use std::io::Error;
use std::path::Path;
use std::path::PathBuf;

extern crate env_logger;
extern crate log;

fn main() {
  // Tell cargo to re-run this script if the env FORCE_REBUILD is set.
  println!("cargo:rerun-if-env-changed=FORCE_REBUILD");
  // Tell cargo to re-run this script if the Rust files have changed.
  println!("cargo:rerun-if-changed=lib.rs");
  println!("cargo:rerun-if-changed=css_parser.rs");
  println!("cargo:rerun-if-changed=layout.rs");
  println!("cargo:rerun-if-changed=typescript_transpiler.rs");
  println!("cargo:rerun-if-changed=glsl_transpiler/");
  // Tell cargo to re-run when the bindings changed.
  println!("cargo:rerun-if-changed=cbindgen.toml");
  println!("cargo:rerun-if-changed=bindings.base.h");
  println!("cargo:rerun-if-changed=bindings.hpp");
  println!("cargo:rerun-if-changed=bindings.css.hpp");
  println!("cargo:rerun-if-changed=bindings.css-inl.hpp");
  println!("cargo:rerun-if-changed=bindings.layout.hpp");
  println!("cargo:rerun-if-changed=bindings.webgl.hpp");

  env_logger::init();

  // Generate C bindings
  generate_holocron();

  // Install headers
  install_header("bindings.base.h");
  install_header("bindings.hpp");
  install_header("bindings.css.hpp");
  install_header("bindings.css-inl.hpp");
  install_header("bindings.layout.hpp");
  install_header("bindings.webgl.hpp");
}

fn generate_holocron_module(source: &str, name: &str, std: &str) {
  cxx_build::bridge(source).std(std).compile(name);

  let out_dir = env::var("OUT_DIR").unwrap();
  let out_path = PathBuf::from(out_dir);
  let out_name = format!("{}.autogen.hpp", name);
  let header = out_path.join(format!("cxxbridge/include/jsar_jsbindings/{}.h", source));
  let _ = std::fs::copy(header, &out_name).unwrap();
  install_header(&out_name);
}

fn generate_holocron() {
  generate_holocron_module("lib.rs", "holocron", "c++20");
  generate_holocron_module("css_parser.rs", "holocron_css_parser", "c++20");
  generate_holocron_module("layout.rs", "holocron_layout", "c++20");
  generate_holocron_module("webgl.rs", "holocron_webgl", "c++20");
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
