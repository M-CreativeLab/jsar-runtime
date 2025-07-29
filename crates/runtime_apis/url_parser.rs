#![allow(unused_variables)]
#![allow(clippy::uninlined_format_args)]
#![allow(deprecated)]

use cxx::CxxString;
use url::Url;

#[cxx::bridge(namespace = "crates")]
mod ffi {
  pub struct URL {
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

  extern "Rust" {
    #[cxx_name = "parseURL"]
    fn parse_url(input: &CxxString) -> Result<URL>;
  }
}

fn parse_url_impl(input_str: &str) -> anyhow::Result<ffi::URL> {
  match Url::parse(input_str) {
    Ok(url) => {
      let hostname = url.host_str().unwrap_or("").to_string();
      let port = url.port().unwrap_or(0);
      let host = format!("{}:{}", hostname, port);
      let origin = url.origin().ascii_serialization();
      let protocol = format!("{}:", url.scheme());

      Ok(ffi::URL {
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
    Err(_) => {
      // If URL parsing fails (e.g., invalid URL, relative URL without base),
      // return a URL struct with empty values to prevent crashes.
      // This allows the calling code to continue execution safely.
      Ok(ffi::URL {
        host: String::new(),
        hostname: String::new(),
        port: 0,
        href: input_str.to_string(), // Keep original input for debugging
        origin: String::new(), // Empty origin will not match any allowed origins
        password: String::new(),
        pathname: String::new(),
        protocol: String::new(),
        search: String::new(),
        username: String::new(),
        hash: String::new(),
      })
    }
  }
}

fn parse_url(input: &CxxString) -> anyhow::Result<ffi::URL> {
  parse_url_impl(input.to_str()?)
}

#[cfg(test)]
mod tests {
  use super::*;

  #[test]
  fn test_parse_valid_url() {
    let result = parse_url_impl("https://example.com/path?query=1#fragment");
    assert!(result.is_ok());
    
    let url = result.unwrap();
    assert_eq!(url.protocol, "https:");
    assert_eq!(url.hostname, "example.com");
    assert_eq!(url.pathname, "/path");
    assert_eq!(url.search, "query=1");
    assert_eq!(url.hash, "fragment");
    assert_eq!(url.href, "https://example.com/path?query=1#fragment");
    assert!(url.origin.contains("https://example.com")); // origin should be set
  }

  #[test]
  fn test_parse_invalid_url_returns_empty_values() {
    // Test various invalid URL inputs that should return empty values instead of failing
    let invalid_inputs = vec![
      "not a url",
      "random text", 
      "file.txt",
      "",
      "://invalid",
      "relative/path",
      "中文测试",
      "test@example",
    ];

    for input in invalid_inputs {
      let result = parse_url_impl(input);
      assert!(result.is_ok(), "Expected '{}' to not fail", input);
      
      let url = result.unwrap();
      // For invalid URLs, most fields should be empty except href which keeps the original input
      assert_eq!(url.origin, "", "Expected empty origin for '{}'", input);
      assert_eq!(url.hostname, "", "Expected empty hostname for '{}'", input);
      assert_eq!(url.protocol, "", "Expected empty protocol for '{}'", input);
      assert_eq!(url.href, input, "Expected href to preserve original input for '{}'", input);
    }
  }

  #[test]
  fn test_parse_relative_url_without_base_returns_empty() {
    // This is the specific case mentioned in the error message
    let result = parse_url_impl("relative/path/to/file");
    assert!(result.is_ok());
    
    let url = result.unwrap();
    assert_eq!(url.origin, ""); // Empty origin - won't match any allowed origins
    assert_eq!(url.hostname, "");
    assert_eq!(url.protocol, "");
    assert_eq!(url.href, "relative/path/to/file"); // Original input preserved
  }
}
