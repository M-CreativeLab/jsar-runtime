use jsar_jsbinding_macro;
use napi_derive;
use napi::{CallContext, Env, JsNumber, JsObject, NapiValue, Result};

#[linked_module(canvas)]
fn init_canvas_module(env: Env, mut exports: JsObject) -> Result<()> {
  Ok(())
}
