#![allow(unused_variables)]
#![allow(clippy::uninlined_format_args)]
#![allow(deprecated)]
#![feature(concat_idents)]

extern crate ctor;
extern crate jsar_jsbinding_macro;
extern crate log;

mod css;
mod css_parser;
mod glsl_transpiler;
mod layout;
mod typescript_transpiler;
mod webgl;

use cxx;
use std::ffi::CString;
use std::os::raw::c_char;
use std::path::Path;
use url::Url;

pub(crate) fn release_cstring(s: *mut c_char) {
  unsafe {
    if s.is_null() {
      return;
    }
    let _ = CString::from_raw(s);
  }
}

pub(crate) fn release_cstring_vec(list: *mut *mut c_char) {
  if list.is_null() {
    return;
  }
  unsafe {
    let mut i = 0;
    while !(*list.offset(i)).is_null() {
      let _ = CString::from_raw(*list.offset(i));
      i += 1;
    }
    let len: usize = i.try_into().unwrap();
    Vec::from_raw_parts(list, len, len);
  }
}

#[no_mangle]
extern "C" fn release_rust_cstring(s: *mut c_char) {
  release_cstring(s)
}

#[no_mangle]
extern "C" fn release_rust_cstrings(list: *mut *mut c_char) {
  release_cstring_vec(list)
}

#[repr(C)]
pub struct WHATWGUrl {
  host: *mut c_char,
  hostname: *mut c_char,
  href: *mut c_char,
  origin: *mut c_char,
  password: *mut c_char,
  pathname: *mut c_char,
  port: i32,
  protocol: *mut c_char,
  search: *mut c_char,
  username: *mut c_char,
  hash: *mut c_char,
}

#[no_mangle]
extern "C" fn parse_whatwg_url(input: *const c_char) -> WHATWGUrl {
  let input_str: &str = unsafe { std::ffi::CStr::from_ptr(input) }
    .to_str()
    .expect("Failed to convert C string to Rust string");
  let url = Url::parse(input_str).expect("Failed to parse URL");
  let hostname = url.host_str().unwrap_or("");
  let port = url.port().unwrap_or(0);
  let host = format!("{}:{}", hostname, port);
  let origin = url.origin().ascii_serialization();
  let protocol = format!("{}:", url.scheme());

  WHATWGUrl {
    host: CString::new(host)
      .expect("Failed to create host CString")
      .into_raw(),
    hostname: CString::new(hostname)
      .expect("Failed to create hostname CString")
      .into_raw(),
    port: port.into(),
    href: CString::new(url.as_str())
      .expect("Failed to create href CString")
      .into_raw(),
    origin: CString::new(origin)
      .expect("Failed to create origin CString")
      .into_raw(),
    password: CString::new(url.password().unwrap_or(""))
      .expect("Failed to create password CString")
      .into_raw(),
    pathname: CString::new(url.path())
      .expect("Failed to create pathname CString")
      .into_raw(),
    protocol: CString::new(protocol)
      .expect("Failed to create protocol CString")
      .into_raw(),
    search: CString::new(url.query().unwrap_or(""))
      .expect("Failed to create search CString")
      .into_raw(),
    username: CString::new(url.username())
      .expect("Failed to create username CString")
      .into_raw(),
    hash: CString::new(url.fragment().unwrap_or(""))
      .expect("Failed to create hash CString")
      .into_raw(),
  }
}

#[no_mangle]
extern "C" fn release_whatwg_url(url: WHATWGUrl) {
  unsafe {
    if !url.host.is_null() {
      let _ = CString::from_raw(url.host);
    }
    if !url.hostname.is_null() {
      let _ = CString::from_raw(url.hostname);
    }
    if !url.href.is_null() {
      let _ = CString::from_raw(url.href);
    }
    if !url.origin.is_null() {
      let _ = CString::from_raw(url.origin);
    }
    if !url.password.is_null() {
      let _ = CString::from_raw(url.password);
    }
    if !url.pathname.is_null() {
      let _ = CString::from_raw(url.pathname);
    }
    if !url.protocol.is_null() {
      let _ = CString::from_raw(url.protocol);
    }
    if !url.search.is_null() {
      let _ = CString::from_raw(url.search);
    }
    if !url.username.is_null() {
      let _ = CString::from_raw(url.username);
    }
    if !url.hash.is_null() {
      let _ = CString::from_raw(url.hash);
    }
  }
}

#[no_mangle]
extern "C" fn create_url_with_path(
  url_str: *const c_char,
  sub_path: *const c_char,
  out_url_str: *mut *mut c_char,
  out_url_max_len: usize,
) -> usize {
  let url_string: &str = unsafe { std::ffi::CStr::from_ptr(url_str) }
    .to_str()
    .expect("Failed to convert C string to Rust string");
  let sub_path_string: &str = unsafe { std::ffi::CStr::from_ptr(sub_path) }
    .to_str()
    .expect("Failed to convert C string to Rust string");

  let url = Url::parse(url_string).unwrap();
  let new_url = url.join(sub_path_string).unwrap();
  let new_url_str = CString::new(new_url.to_string()).expect("Failed to create URL CString");

  let new_url_len = new_url_str.as_bytes().len();
  if new_url_len > out_url_max_len {
    0
  } else {
    // Copy the new URL string to the output buffer.
    unsafe {
      std::ptr::copy_nonoverlapping(
        new_url_str.as_ptr(),
        out_url_str as *mut c_char,
        new_url_len,
      );
    }
    new_url_len
  }
}

#[repr(C)]
pub struct TranspiledTypeScriptOutput {
  code: *mut c_char,
  error_message: *mut c_char,
}

#[no_mangle]
extern "C" fn transpile_typescript_to_js(input_str: *const c_char) -> TranspiledTypeScriptOutput {
  let input_string: &str = unsafe { std::ffi::CStr::from_ptr(input_str) }
    .to_str()
    .expect("Failed to read TypeScript input string");

  let output = typescript_transpiler::transpile_typescript_to_js(input_string.into());
  if let Err(e) = output {
    let error_message = format!("Error: {:?}", e);
    let error_message_cstring =
      CString::new(error_message).expect("Failed to create error CString");
    TranspiledTypeScriptOutput {
      code: std::ptr::null_mut(),
      error_message: error_message_cstring.into_raw(),
    }
  } else {
    let output_string = output.unwrap().code;
    let output_cstring = CString::new(output_string).expect("Failed to create output CString");
    TranspiledTypeScriptOutput {
      code: output_cstring.into_raw(),
      error_message: std::ptr::null_mut(),
    }
  }
}

#[no_mangle]
extern "C" fn release_transpiled_typescript_output(output: TranspiledTypeScriptOutput) {
  unsafe {
    if !output.code.is_null() {
      let _ = CString::from_raw(output.code);
    }
    if !output.error_message.is_null() {
      let _ = CString::from_raw(output.error_message);
    }
  }
}

#[cxx::bridge(namespace = "holocron")]
mod ffi {
  enum ModuleExtensionIndex {
    #[cxx_name = "kNone"]
    None = 0,
    #[cxx_name = "kJavaScript"]
    JavaScript,
    #[cxx_name = "kTypeScript"]
    TypeScript,
    #[cxx_name = "kJSON"]
    JSON,
    #[cxx_name = "kBin"]
    Bin,
    #[cxx_name = "kData"]
    Data,
    #[cxx_name = "kWebAssembly"]
    WebAssembly,
    #[cxx_name = "kPNG"]
    PNG,
    #[cxx_name = "kJPEG"]
    JPEG,
    #[cxx_name = "kGIF"]
    GIF,
    #[cxx_name = "kSVG"]
    SVG,
    #[cxx_name = "kMP3"]
    MP3,
    #[cxx_name = "kWAV"]
    WAV,
    #[cxx_name = "kOGG"]
    OGG,
  }

  extern "Rust" {
    #[cxx_name = "parseURLToModuleExtension"]
    fn parse_url_to_module_extension(url: &str) -> ModuleExtensionIndex;
  }
}

impl std::fmt::Debug for ffi::ModuleExtensionIndex {
  fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
    match *self {
      ffi::ModuleExtensionIndex::None => write!(f, "None"),
      ffi::ModuleExtensionIndex::JavaScript => write!(f, "JavaScript"),
      ffi::ModuleExtensionIndex::TypeScript => write!(f, "TypeScript"),
      ffi::ModuleExtensionIndex::JSON => write!(f, "JSON"),
      ffi::ModuleExtensionIndex::Bin => write!(f, "Bin"),
      ffi::ModuleExtensionIndex::Data => write!(f, "Data"),
      ffi::ModuleExtensionIndex::WebAssembly => write!(f, "WebAssembly"),
      ffi::ModuleExtensionIndex::PNG => write!(f, "PNG"),
      ffi::ModuleExtensionIndex::JPEG => write!(f, "JPEG"),
      ffi::ModuleExtensionIndex::GIF => write!(f, "GIF"),
      ffi::ModuleExtensionIndex::SVG => write!(f, "SVG"),
      ffi::ModuleExtensionIndex::MP3 => write!(f, "MP3"),
      ffi::ModuleExtensionIndex::WAV => write!(f, "WAV"),
      ffi::ModuleExtensionIndex::OGG => write!(f, "OGG"),
      _ => write!(f, "None"),
    }
  }
}

fn parse_url_to_module_extension(url_str: &str) -> ffi::ModuleExtensionIndex {
  let url = Url::parse(url_str).unwrap();

  // Check if the URL path ends with a file extension like .html, .css, etc.
  if let Some(file_extension) = url.path().rsplit('.').next() {
    match file_extension {
      "js" | "mjs" => ffi::ModuleExtensionIndex::JavaScript,
      "ts" => ffi::ModuleExtensionIndex::TypeScript,
      "json" => ffi::ModuleExtensionIndex::JSON,
      "bin" => ffi::ModuleExtensionIndex::Bin,
      "data" => ffi::ModuleExtensionIndex::Data,
      "wasm" => ffi::ModuleExtensionIndex::WebAssembly,
      "png" => ffi::ModuleExtensionIndex::PNG,
      "jpg" | "jpeg" => ffi::ModuleExtensionIndex::JPEG,
      "gif" => ffi::ModuleExtensionIndex::GIF,
      "svg" => ffi::ModuleExtensionIndex::SVG,
      "mp3" => ffi::ModuleExtensionIndex::MP3,
      "wav" => ffi::ModuleExtensionIndex::WAV,
      "ogg" => ffi::ModuleExtensionIndex::OGG,
      _ => ffi::ModuleExtensionIndex::None,
    }
  } else {
    ffi::ModuleExtensionIndex::None
  }
}

#[cfg(test)]
mod tests {
  use super::*;
  use std::ffi::CString;

  #[test]
  fn test_parse_whatwg_url() {
    let url_str = CString::new("https://example.com:8080/path?query#fragment").unwrap();
    let url = parse_whatwg_url(url_str.as_ptr());
    assert_eq!(
      unsafe { CString::from_raw(url.host).to_str().unwrap() },
      "example.com:8080"
    );
    assert_eq!(
      unsafe { CString::from_raw(url.hostname).to_str().unwrap() },
      "example.com"
    );
    assert_eq!(url.port, 8080);
    assert_eq!(
      unsafe { CString::from_raw(url.href).to_str().unwrap() },
      "https://example.com:8080/path?query#fragment"
    );
    assert_eq!(
      unsafe { CString::from_raw(url.origin).to_str().unwrap() },
      "https://example.com:8080"
    );
    assert_eq!(
      unsafe { CString::from_raw(url.password).to_str().unwrap() },
      ""
    );
    assert_eq!(
      unsafe { CString::from_raw(url.pathname).to_str().unwrap() },
      "/path"
    );
    assert_eq!(
      unsafe { CString::from_raw(url.protocol).to_str().unwrap() },
      "https:"
    );
    assert_eq!(
      unsafe { CString::from_raw(url.search).to_str().unwrap() },
      "query"
    );
    assert_eq!(
      unsafe { CString::from_raw(url.username).to_str().unwrap() },
      ""
    );
    assert_eq!(
      unsafe { CString::from_raw(url.hash).to_str().unwrap() },
      "fragment"
    );
  }

  #[test]
  fn test_parse_url_to_module_extension() {
    let extension = parse_url_to_module_extension("https://example.com/index.js");
    assert_eq!(extension, ffi::ModuleExtensionIndex::JavaScript);

    let extension = parse_url_to_module_extension("https://example.com/index.ts");
    assert_eq!(extension, ffi::ModuleExtensionIndex::TypeScript);

    let extension = parse_url_to_module_extension("https://example.com/data.json");
    assert_eq!(extension, ffi::ModuleExtensionIndex::JSON);

    let extension = parse_url_to_module_extension("https://example.com/file.bin");
    assert_eq!(extension, ffi::ModuleExtensionIndex::Bin);

    let extension = parse_url_to_module_extension("https://example.com/file.data");
    assert_eq!(extension, ffi::ModuleExtensionIndex::Data);

    let extension = parse_url_to_module_extension("https://example.com/file.wasm");
    assert_eq!(extension, ffi::ModuleExtensionIndex::WebAssembly);

    let extension = parse_url_to_module_extension("https://example.com/image.png");
    assert_eq!(extension, ffi::ModuleExtensionIndex::PNG);

    let extension = parse_url_to_module_extension("https://example.com/image.jpg");
    assert_eq!(extension, ffi::ModuleExtensionIndex::JPEG);

    let extension = parse_url_to_module_extension("https://example.com/image.gif");
    assert_eq!(extension, ffi::ModuleExtensionIndex::GIF);

    let extension = parse_url_to_module_extension("https://example.com/image.svg");
    assert_eq!(extension, ffi::ModuleExtensionIndex::SVG);

    let extension = parse_url_to_module_extension("https://example.com/audio.mp3");
    assert_eq!(extension, ffi::ModuleExtensionIndex::MP3);

    let extension = parse_url_to_module_extension("https://example.com/audio.wav");
    assert_eq!(extension, ffi::ModuleExtensionIndex::WAV);

    let extension = parse_url_to_module_extension("https://example.com/audio.ogg");
    assert_eq!(extension, ffi::ModuleExtensionIndex::OGG);

    let extension = parse_url_to_module_extension("https://example.com/unknown");
    assert_eq!(extension, ffi::ModuleExtensionIndex::None);
  }
}
