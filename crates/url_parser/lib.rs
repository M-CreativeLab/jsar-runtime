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
  let url = Url::parse(input_str)?;

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

fn parse_url(input: &CxxString) -> anyhow::Result<ffi::URL> {
  parse_url_impl(input.to_str()?)
}
