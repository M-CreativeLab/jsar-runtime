use std::env;
use std::fs;
use std::io::Error;
use std::path::Path;
use std::path::PathBuf;

fn main() {
  install_script("jsar-bootstrap.js");
  install_script("jsar-runtime.js");
  install_header("jsar_jsbundle.h");
}

fn install_script(file_name: &str) {
  let current_dir = env::current_dir().unwrap();
  let gzipped_file_name = file_name.to_string() + ".gz";
  let source_path: std::path::PathBuf = current_dir.join("../../build/output").join(gzipped_file_name.clone());
  println!("cargo:rerun-if-changed={}", source_path.display());

  if !source_path.exists() {
    panic!(
      "Error: File '{}' not found in directory '{}'",
      file_name,
      source_path.to_str().unwrap()
    );
  }

  let destination_path = current_dir.join(gzipped_file_name.clone());
  match copy_file(&source_path, &destination_path) {
    Ok(_) => println!(
      "File '{}' copied to '{}'",
      file_name,
      destination_path.to_str().unwrap()
    ),
    Err(e) => panic!("Error copying file: {:?}", e),
  }
}

fn install_header(file_name: &str) {
  let current_dir = env::current_dir().unwrap();
  let source_path = current_dir.join(file_name);
  println!("cargo:rerun-if-changed={}", source_path.display());

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
  let content = fs::read(source)?;
  fs::write(destination, content)?;
  Ok(())
}
