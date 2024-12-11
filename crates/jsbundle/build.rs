use md5::{Digest, Md5};
use std::env;
use std::fs;
use std::io::Error;
use std::path::Path;
use std::path::PathBuf;

fn main() {
  install_nodejs_library();
  install_script("jsar-bootstrap.js");
  install_script("jsar-runtime.js");
  install_header("jsar_jsbundle.h");
}

fn install_nodejs_library() {
  let target = env::var("TARGET").unwrap();
  let library_prefix: &str;
  let library_name: &str;
  let dest_dir: &str;

  if target.contains("apple") {
    library_prefix = "Darwin/lib";
    library_name = "libnode.108.dylib";
    dest_dir = "darwin";
  } else if target.contains("android") {
    library_prefix = "Android/aarch64/lib";
    library_name = "libnode.so";
    dest_dir = "android-aarch64";
  } else if target.contains("windows") {
    library_prefix = "Windows/x64_64/lib";
    library_name = "node.lib";
    dest_dir = "windows-x64_64";
  } else {
    panic!("Unsupported target: {}", target);
  }

  let current_dir = env::current_dir().unwrap();
  let source_path: std::path::PathBuf = current_dir
    .join("../../thirdparty/libs")
    .join(library_prefix)
    .join(library_name.clone());
  println!("cargo:rerun-if-changed={}", source_path.display());

  if !source_path.exists() {
    panic!(
      "Error: File '{}' not found in directory '{}'",
      library_name,
      source_path.to_str().unwrap()
    );
  }

  let destination_dir = current_dir.join("res");
  if !destination_dir.exists() {
    std::fs::create_dir_all(&destination_dir).expect("Failed to create the destination directory.");
  }
  let destination_path = destination_dir.join("libnode.lib");
  match copy_file(&source_path, &destination_path, true) {
    Ok(_) => println!(
      "File '{}' copied to '{}'",
      library_name,
      destination_path.to_str().unwrap()
    ),
    Err(e) => panic!("Error copying file: {:?}", e),
  }
}

fn install_script(file_name: &str) {
  let current_dir = env::current_dir().unwrap();
  let gzipped_file_name = file_name.to_string() + ".gz";
  let source_path: std::path::PathBuf = current_dir
    .join("../../build/output")
    .join(gzipped_file_name.clone());
  println!("cargo:rerun-if-changed={}", source_path.display());

  if !source_path.exists() {
    panic!(
      "Error: File '{}' not found in directory '{}'",
      file_name,
      source_path.to_str().unwrap()
    );
  }

  let destination_path = current_dir.join(gzipped_file_name.clone());
  match copy_file(&source_path, &destination_path, false) {
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
  match copy_file(&source_path, &destination_path, false) {
    Ok(_) => println!(
      "File '{}' copied to '{}'",
      file_name,
      destination_path.to_str().unwrap()
    ),
    Err(e) => panic!("Error copying file: {:?}", e),
  }
}

fn copy_file(source: &Path, destination: &Path, write_md5: bool) -> Result<(), Error> {
  let content = fs::read(source)?;
  fs::write(destination, content.clone())?;

  if write_md5 {
    // Write the md5 hash of the file to destination + ".md5"
    let mut hasher = Md5::new();
    hasher.update(&content);

    let md5_hash = hasher.finalize();
    let md5_file = destination.to_str().unwrap().to_string() + ".md5";
    fs::write(md5_file, format!("{:x}", md5_hash))?;
  }
  Ok(())
}
