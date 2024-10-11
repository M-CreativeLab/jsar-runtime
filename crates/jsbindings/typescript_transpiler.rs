use anyhow::Error;
use swc_common::{errors::ColorConfig, sync::Lrc, SourceMap};
use swc_error_reporters::handler::{try_with_handler, HandlerOpts};
use swc_fast_ts_strip::{Options, TransformOutput};

pub fn transpile_typescript_to_js(input: String) -> Result<TransformOutput, Error> {
  let cm: Lrc<SourceMap> = Default::default();
  let mut options: Options = Default::default();
  options.source_map = true;

  try_with_handler(
    cm.clone(),
    HandlerOpts {
      color: ColorConfig::Never,
      skip_filename: false,
    },
    |handler| swc_fast_ts_strip::operate(&cm, handler, input, options),
  )
}

#[test]
fn test_transpile_typescript_to_js() {
  let input = r#"
interface IBar {
  bar: string;
}
class Foo implements IBar {
  private foo: string;
  bar: string;
  constructor() {
    console.log("Hello, World!");
  }
}
const a: number = 1;
const b: number = 2;
const c: number = a + b;
console.log(c);
  "#
  .to_string();

  let out = transpile_typescript_to_js(input);
  if let Err(e) = out {
    println!("Error: {:?}", e);
  } else {
    let _out = out.unwrap();
    println!("Output: {}", _out.code);
    println!("Map: {:?}", _out.map);
  }
}
