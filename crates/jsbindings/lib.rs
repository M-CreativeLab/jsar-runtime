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
mod texture_atlas;
mod typescript_transpiler;
mod webgl;

use cxx::CxxString;
use url::Url;

#[cxx::bridge(namespace = "holocron")]
mod ffi {
  struct WHATWGUrl {
    host: String,
    hostname: String,
    href: String,
    origin: String,
    password: String,
    pathname: String,
    port: i32,
    protocol: String,
    search: String,
    username: String,
    hash: String,
  }

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

  struct TranspiledScriptSource {
    code: String,
    error: String,
  }

  extern "Rust" {
    #[cxx_name = "parseWHATWGUrl"]
    fn parse_whatwg_url(input: &CxxString) -> Result<WHATWGUrl>;

    #[cxx_name = "parseURLToModuleExtension"]
    fn parse_url_to_module_extension(url: &str) -> ModuleExtensionIndex;

    #[cxx_name = "createUrlWithPath"]
    fn create_url_with_path(url: &CxxString, sub_path: &CxxString) -> String;
  }

  extern "Rust" {
    #[cxx_name = "transpileTypeScript"]
    fn transpile_typescript(src: &CxxString) -> Result<TranspiledScriptSource>;
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

fn parse_whatwg_url_impl(input_str: &str) -> anyhow::Result<ffi::WHATWGUrl> {
  let url = Url::parse(input_str)?;

  let hostname = url.host_str().unwrap_or("").to_string();
  let port = url.port().unwrap_or(0);
  let host = format!("{}:{}", hostname, port);
  let origin = url.origin().ascii_serialization();
  let protocol = format!("{}:", url.scheme());

  Ok(ffi::WHATWGUrl {
    host,
    hostname,
    port: port.into(),
    href: url.as_str().to_string(),
    origin,
    password: url.password().unwrap_or("").to_string(),
    pathname: url.path().to_string(),
    protocol,
    search: url.query().unwrap_or("").to_string(),
    username: url.username().to_string(),
    hash: url.fragment().unwrap_or("").to_string(),
  })
}

fn parse_whatwg_url(input: &CxxString) -> anyhow::Result<ffi::WHATWGUrl> {
  parse_whatwg_url_impl(input.to_str()?)
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

fn create_url_with_path(url: &CxxString, sub_path: &CxxString) -> String {
  let url_str = url
    .to_str()
    .expect("Failed to convert C string to Rust string");
  let sub_path_str = sub_path
    .to_str()
    .expect("Failed to convert C string to Rust string");

  let url = Url::parse(url_str).unwrap();
  let new_url = url.join(sub_path_str).unwrap();
  new_url.to_string()
}

fn transpile_typescript(src: &CxxString) -> anyhow::Result<ffi::TranspiledScriptSource> {
  let input_str = src.to_str()?;
  let output = typescript_transpiler::transpile_typescript_to_js(input_str.into());
  if let Err(e) = output {
    let error = format!("Error: {:?}", e);
    Ok(ffi::TranspiledScriptSource {
      code: String::new(),
      error,
    })
  } else {
    let output_string = output.unwrap().code;
    Ok(ffi::TranspiledScriptSource {
      code: output_string,
      error: String::new(),
    })
  }
}

#[cfg(test)]
mod tests {
  use super::*;

  #[test]
  fn test_parse_whatwg_url() {
    let input_str = "https://example.com:8080/path?query#fragment";
    let url = parse_whatwg_url_impl(input_str).unwrap();
    assert_eq!(url.host, "example.com:8080");
    assert_eq!(url.hostname, "example.com");
    assert_eq!(url.port, 8080);
    assert_eq!(url.href, "https://example.com:8080/path?query#fragment");
    assert_eq!(url.origin, "https://example.com:8080");
    assert_eq!(url.password, "");
    assert_eq!(url.pathname, "/path");
    assert_eq!(url.protocol, "https:");
    assert_eq!(url.search, "query");
    assert_eq!(url.username, "");
    assert_eq!(url.hash, "fragment");
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
