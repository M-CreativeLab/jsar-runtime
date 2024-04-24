use euclid::default::Size2D;
use napi::{CallContext, Env, JsNumber, JsObject, JsUndefined, NapiValue, Property, Result};
use surfman::ContextAttributeFlags;

#[js_function(2)]
fn htmlrender_constructor(ctx: CallContext) -> Result<JsUndefined> {
  if ctx.length < 2 {
    ctx.env.throw_type_error(
      "HTMLRender.constructor requires 2 arguments",
      Some("ArgumentError"),
    )?;
    return ctx.env.get_undefined();
  }
  let width = ctx.get::<JsNumber>(0)?.get_int32()?;
  let height = ctx.get::<JsNumber>(1)?.get_int32()?;
  let mut this: JsObject = ctx.this_unchecked();
  this.set_named_property("width", ctx.env.create_int32(width)?)?;
  this.set_named_property("height", ctx.env.create_int32(height)?)?;

  let connection = surfman::Connection::new().expect("Failed to create a connection");
  let adapter = connection
    .create_low_power_adapter()
    .expect("Failed to create an adapter");
  let mut device = connection
    .create_device(&adapter)
    .expect("Failed to create a device");
  let context_descriptor = device
    .create_context_descriptor(&surfman::ContextAttributes {
      version: surfman::GLVersion::new(3, 0),
      flags: ContextAttributeFlags::empty(),
    })
    .expect("Failed to create a context descriptor");
  let mut context = device
    .create_context(&context_descriptor, None)
    .expect("Failed to create a context");
  trace!("created surface context already(js)");

  let surface = device
    .create_surface(
      &context,
      surfman::SurfaceAccess::GPUOnly,
      surfman::SurfaceType::Generic {
        size: Size2D::new(width, height),
      },
    )
    .expect("Failed to create a surface");
  trace!("created surface(js).");

  device
    .bind_surface_to_context(&mut context, surface)
    .expect("Failed to bind surface to context");
  device
    .make_context_current(&context)
    .expect("Failed to make context current");
  gl::load_with(|symbol| device.get_proc_address(&context, symbol) as *const _);
  trace!("binded surface to context and make the context alive");

  let framebuffer_object = device
    .context_surface_info(&context)
    .unwrap()
    .unwrap()
    .framebuffer_object;
  trace!("framebuffer_object(js) is {:?}", framebuffer_object);

  // Create webrender instance
  {
    // Do initialize the webrender instance.
  }

  let old_surface = device
    .unbind_surface_from_context(&mut context)
    .expect("Failed to unbind surface from context")
    .unwrap();

  let texture = device
    .create_surface_texture(&mut context, old_surface)
    .unwrap();
  let gl_texture = device.surface_texture_object(&texture);

  trace!("gl_texture(js) is {:?}", gl_texture);
  let mut surface_to_destroy = device
    .destroy_surface_texture(&mut context, texture)
    .expect("Failed to destroy texture");
  device
    .destroy_surface(&mut context, &mut surface_to_destroy)
    .expect("Failed to destroy surface");
  device
    .destroy_context(&mut context)
    .expect("Failed to destroy context");

  ctx.env.get_undefined()
}

#[js_function]
fn htmlrender_height_getter(ctx: CallContext) -> Result<JsNumber> {
  let this: JsObject = ctx.this_unchecked();
  let height = this.get_named_property::<JsNumber>("height")?.get_int32()?;
  ctx.env.create_int32(height)
}

#[js_function(1)]
fn htmlrender_height_setter(ctx: CallContext) -> Result<JsUndefined> {
  let mut this: JsObject = ctx.this_unchecked();
  let height = ctx.get::<JsNumber>(0)?.get_int32()?;
  this.set_named_property("height", ctx.env.create_int32(height)?)?;
  ctx.env.get_undefined()
}

#[js_function]
fn htmlrender_width_getter(ctx: CallContext) -> Result<JsNumber> {
  let this: JsObject = ctx.this_unchecked();
  let width = this.get_named_property::<JsNumber>("width")?.get_int32()?;
  ctx.env.create_int32(width)
}

#[js_function(1)]
fn htmlrender_width_setter(ctx: CallContext) -> Result<JsUndefined> {
  let mut this: JsObject = ctx.this_unchecked();
  let width = ctx.get::<JsNumber>(0)?.get_int32()?;
  this.set_named_property("width", ctx.env.create_int32(width)?)?;
  ctx.env.get_undefined()
}

#[linked_module(htmlrender)]
fn init_htmlrender_module(env: Env, mut exports: JsObject) -> Result<()> {
  let htmlrender_class = env.define_class(
    "HTMLRender",
    htmlrender_constructor,
    &[
      Property::new("height")?
        .with_getter(htmlrender_height_getter)
        .with_setter(htmlrender_height_setter),
      Property::new("width")?
        .with_getter(htmlrender_width_getter)
        .with_setter(htmlrender_width_setter),
    ],
  )?;
  exports.set_named_property("HTMLRender", htmlrender_class)?;
  Ok(())
}
