use naga::back::msl;
use naga::front::glsl::{Frontend, Options};
use naga::ShaderStage;

#[no_mangle]
pub extern "C" fn glsl_to_msl(
  stage: u8,
  input_ptr: *const u8,
  input_size: usize,
  output_ptr: *mut *const u8,
  output_size: *mut usize,
) -> usize {
  let input_bytes = unsafe { std::slice::from_raw_parts(input_ptr, input_size) };
  let input = std::str::from_utf8(input_bytes).unwrap();

  let mut parser: Frontend = Frontend::default();
  let parsed = parser
    .parse(
      &naga::front::glsl::Options {
        stage: match stage {
          0 => naga::ShaderStage::Vertex,
          1 => naga::ShaderStage::Fragment,
          2 => naga::ShaderStage::Compute,
          _ => panic!("Unknown stage: {}", stage),
        },
        defines: Default::default(),
      },
      &input,
    )
    .unwrap_or_else(|err| {
      for e in err.iter() {
        eprintln!("error: {}", e);
      }
      panic!("Failed to parse GLSL, errors has been printed above.");
    });

  let info = match naga::valid::Validator::new(
    naga::valid::ValidationFlags::all(),
    naga::valid::Capabilities::all(),
  )
  .validate(&parsed)
  {
    Ok(info) => Some(info),
    Err(error) => {
      panic!("Validation failed: {}", error);
      None
    }
  }
  .unwrap();

  // write to MSL
  let mut options = msl::Options::default();
  let pipeline_opts = msl::PipelineOptions::default();
  let (msl, _) = msl::write_string(&parsed, &info, &options, &pipeline_opts).unwrap();

  // create a new char* and copy the string into it
  unsafe {
    *output_size = msl.len();
    let c_str = std::ffi::CString::new(msl).unwrap();
    let ptr = c_str.into_raw() as *const u8;
    *output_ptr = ptr;
  }
  0
}
