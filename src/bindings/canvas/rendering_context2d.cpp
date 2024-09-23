#include "./rendering_context2d-inl.hpp"

namespace canvasbinding
{
  Napi::FunctionReference *CanvasRenderingContext2D::constructor;
  void CanvasRenderingContext2D::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "CanvasRenderingContext2D", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("CanvasRenderingContext2D", func);
  }

  Napi::Object CanvasRenderingContext2D::NewInstance(Napi::Env env,
                                                     shared_ptr<canvas::RenderingContextBase<canvas::Canvas>> contextImpl,
                                                     Napi::Value canvasValue)
  {
    return CanvasRenderingContext2DBase<CanvasRenderingContext2D, canvas::Canvas>::NewInstance(env, contextImpl, canvasValue);
  }

  std::vector<Napi::ClassPropertyDescriptor<OffscreenCanvasRenderingContext2D>> OffscreenCanvasRenderingContext2D::GetClassProperties()
  {
    using T = OffscreenCanvasRenderingContext2D;
    auto props = CanvasRenderingContext2DBase<OffscreenCanvasRenderingContext2D, canvas::OffscreenCanvas>::GetClassProperties();
    auto added = vector<Napi::ClassPropertyDescriptor<OffscreenCanvasRenderingContext2D>>(
        {
            T::InstanceMethod("commit", &T::Commit),
        });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  Napi::Object OffscreenCanvasRenderingContext2D::NewInstance(Napi::Env env,
                                                              shared_ptr<canvas::RenderingContextBase<canvas::OffscreenCanvas>> contextImpl,
                                                              Napi::Value canvasValue)
  {
    return CanvasRenderingContext2DBase<OffscreenCanvasRenderingContext2D, canvas::OffscreenCanvas>::NewInstance(env, contextImpl, canvasValue);
  }

  Napi::FunctionReference *OffscreenCanvasRenderingContext2D::constructor;
  void OffscreenCanvasRenderingContext2D::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "OffscreenCanvasRenderingContext2D", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("OffscreenCanvasRenderingContext2D", func);
  }

  Napi::Value OffscreenCanvasRenderingContext2D::Commit(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::TypeError::New(env, "Not implemented yet").ThrowAsJavaScriptException();
    return env.Undefined();
  }
}
