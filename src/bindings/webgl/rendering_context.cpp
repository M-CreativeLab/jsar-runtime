#include "rendering_context.hpp"

namespace webgl
{
  Napi::FunctionReference *WebGLRenderingContext::constructor;

  Napi::Object WebGLRenderingContext::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(
        env,
        "WebGLRenderingContext",
        {// variables
         InstanceValue("COLOR_BUFFER_BIT", Napi::Number::New(env, COLOR_BUFFER_BIT)),
         InstanceValue("DEPTH_BUFFER_BIT", Napi::Number::New(env, DEPTH_BUFFER_BIT)),
         InstanceValue("STENCIL_BUFFER_BIT", Napi::Number::New(env, STENCIL_BUFFER_BIT)),
         InstanceValue("VERTEX_SHADER", Napi::Number::New(env, WEBGL_VERTEX_SHADER)),
         InstanceValue("FRAGMENT_SHADER", Napi::Number::New(env, WEBGL_FRAGMENT_SHADER)),
         // instance methods
         InstanceMethod("createProgram", &WebGLRenderingContext::CreateProgram),
         InstanceMethod("linkProgram", &WebGLRenderingContext::LinkProgram),
         InstanceMethod("useProgram", &WebGLRenderingContext::UseProgram),
         InstanceMethod("attachShader", &WebGLRenderingContext::AttachShader),
         InstanceMethod("detachShader", &WebGLRenderingContext::DetachShader),
         InstanceMethod("createShader", &WebGLRenderingContext::CreateShader),
         InstanceMethod("deleteShader", &WebGLRenderingContext::DeleteShader),
         InstanceMethod("shaderSource", &WebGLRenderingContext::ShaderSource),
         InstanceMethod("compileShader", &WebGLRenderingContext::CompileShader),
         InstanceMethod("createBuffer", &WebGLRenderingContext::CreateBuffer),
         InstanceMethod("bindBuffer", &WebGLRenderingContext::BindBuffer),
         InstanceMethod("enableVertexAttribArray", &WebGLRenderingContext::EnableVertexAttribArray),
         InstanceMethod("vertexAttribPointer", &WebGLRenderingContext::VertexAttribPointer),
         InstanceMethod("drawArrays", &WebGLRenderingContext::DrawArrays),
         InstanceMethod("drawElements", &WebGLRenderingContext::DrawElements),
         InstanceMethod("viewport", &WebGLRenderingContext::Viewport),
         InstanceMethod("clearColor", &WebGLRenderingContext::ClearColor),
         InstanceMethod("clearDepth", &WebGLRenderingContext::ClearDepth),
         InstanceMethod("clearStencil", &WebGLRenderingContext::ClearStencil),
         InstanceMethod("clear", &WebGLRenderingContext::Clear),
         InstanceMethod("enable", &WebGLRenderingContext::Enable),
         InstanceMethod("scissor", &WebGLRenderingContext::Scissor),
         // getter & setter
         InstanceAccessor<&WebGLRenderingContext::DrawingBufferWidthGetter, &WebGLRenderingContext::DrawingBufferWidthSetter>("drawingBufferWidth"),
         InstanceAccessor<&WebGLRenderingContext::DrawingBufferHeightGetter, &WebGLRenderingContext::DrawingBufferHeightSetter>("drawingBufferHeight")});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);

    exports.Set("WebGLRenderingContext", tpl);
    return exports;
  }

  WebGLRenderingContext::WebGLRenderingContext(const Napi::CallbackInfo &info) : Napi::ObjectWrap<WebGLRenderingContext>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    m_renderAPI = RenderAPI::Get();
    if (m_renderAPI == nullptr)
    {
      Napi::TypeError::New(env, "RenderAPI is not available").ThrowAsJavaScriptException();
      return;
    }
  }

  Napi::Value WebGLRenderingContext::CreateProgram(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int program = m_renderAPI->CreateProgram();
    return Napi::Number::New(env, program);
  }

  Napi::Value WebGLRenderingContext::LinkProgram(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "linkProgram() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int program = info[0].As<Napi::Number>().Int32Value();
    m_renderAPI->LinkProgram(program);
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::UseProgram(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "useProgram() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int program = info[0].As<Napi::Number>().Int32Value();
    m_renderAPI->UseProgram(program);
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::AttachShader(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "attachShader() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int program = info[0].As<Napi::Number>().Int32Value();
    int shader = info[1].As<Napi::Number>().Int32Value();
    m_renderAPI->AttachShader(program, shader);
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::DetachShader(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "detachShader() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int program = info[0].As<Napi::Number>().Int32Value();
    int shader = info[1].As<Napi::Number>().Int32Value();
    m_renderAPI->DetachShader(program, shader);
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::CreateShader(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "createShader() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int type = info[0].As<Napi::Number>().Int32Value();
    int shader = m_renderAPI->CreateShader(type);
    return Napi::Number::New(env, shader);
  }

  Napi::Value WebGLRenderingContext::DeleteShader(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "deleteShader() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int shader = info[0].As<Napi::Number>().Int32Value();
    m_renderAPI->DeleteShader(shader);
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::ShaderSource(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "shaderSource() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int shader = info[0].As<Napi::Number>().Int32Value();
    std::string source = info[1].As<Napi::String>().Utf8Value();
    m_renderAPI->ShaderSource(shader, source.c_str(), source.length());
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::CompileShader(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "compileShader() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int shader = info[0].As<Napi::Number>().Int32Value();
    m_renderAPI->CompileShader(shader);
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::CreateBuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int buffer = m_renderAPI->CreateBuffer();
    return Napi::Number::New(env, buffer);
  }

  Napi::Value WebGLRenderingContext::BindBuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "bindBuffer() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int target = info[0].As<Napi::Number>().Int32Value();
    int buffer = info[1].As<Napi::Number>().Int32Value();
    m_renderAPI->BindBuffer(target, buffer);
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::EnableVertexAttribArray(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "enableVertexAttribArray() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int index = info[0].As<Napi::Number>().Int32Value();
    m_renderAPI->EnableVertexAttribArray(index);
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::VertexAttribPointer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // if (info.Length() < 6)
    // {
    //   Napi::TypeError::New(env, "vertexAttribPointer() takes 6 arguments.").ThrowAsJavaScriptException();
    //   return env.Undefined();
    // }
    // int index = info[0].As<Napi::Number>().Int32Value();
    // int size = info[1].As<Napi::Number>().Int32Value();
    // int type = info[2].As<Napi::Number>().Int32Value();
    // bool normalized = info[3].As<Napi::Boolean>().Value();
    // int stride = info[4].As<Napi::Number>().Int32Value();
    // int offset = info[5].As<Napi::Number>().Int32Value();
    // m_renderAPI->VertexAttribPointer(index, size, type, normalized, stride, (const void *)offset);
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::DrawArrays(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "drawArrays() takes 3 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int mode = info[0].As<Napi::Number>().Int32Value();
    int first = info[1].As<Napi::Number>().Int32Value();
    int count = info[2].As<Napi::Number>().Int32Value();
    m_renderAPI->DrawArrays(mode, first, count);
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::DrawElements(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // if (info.Length() < 4)
    // {
    //   Napi::TypeError::New(env, "drawElements() takes 4 arguments.").ThrowAsJavaScriptException();
    //   return env.Undefined();
    // }
    // int mode = info[0].As<Napi::Number>().Int32Value();
    // int count = info[1].As<Napi::Number>().Int32Value();
    // int type = info[2].As<Napi::Number>().Int32Value();
    // int offset = info[3].As<Napi::Number>().Int32Value();
    // m_renderAPI->DrawElements(mode, count, type, (const void *)offset);
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::Viewport(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 4)
    {
      Napi::TypeError::New(env, "viewport() takes 4 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int x = info[0].As<Napi::Number>().Int32Value();
    int y = info[1].As<Napi::Number>().Int32Value();
    int width = info[2].As<Napi::Number>().Int32Value();
    int height = info[3].As<Napi::Number>().Int32Value();
    m_renderAPI->SetViewport(x, y, width, height);
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::ClearColor(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 4)
    {
      float r = info[0].ToNumber().FloatValue();
      float g = info[1].ToNumber().FloatValue();
      float b = info[2].ToNumber().FloatValue();
      float a = info[3].ToNumber().FloatValue();
      m_renderAPI->ClearColor(r, g, b, a);
    }
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::ClearDepth(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1)
    {
      float depth = info[0].ToNumber().FloatValue();
      m_renderAPI->ClearDepth(depth);
    }
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::ClearStencil(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1)
    {
      int stencil = info[0].ToNumber().Int32Value();
      m_renderAPI->ClearStencil(stencil);
    }
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::Clear(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1)
    {
      uint32_t mask = info[0].ToNumber().Uint32Value();
      m_renderAPI->Clear(mask);
    }
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::Enable(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1)
    {
      uint32_t mask = info[0].ToNumber().Uint32Value();
      m_renderAPI->Enable(mask);
    }
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::Scissor(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 4)
    {
      Napi::TypeError::New(env, "viewport() takes 4 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int x = info[0].As<Napi::Number>().Int32Value();
    int y = info[1].As<Napi::Number>().Int32Value();
    int width = info[2].As<Napi::Number>().Int32Value();
    int height = info[3].As<Napi::Number>().Int32Value();
    m_renderAPI->SetScissor(x, y, width, height);
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::DrawingBufferWidthGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, m_renderAPI->GetDrawingBufferWidth());
  }

  void WebGLRenderingContext::DrawingBufferWidthSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::TypeError::New(env, "drawingBufferWidth is readonly.").ThrowAsJavaScriptException();
  }

  Napi::Value WebGLRenderingContext::DrawingBufferHeightGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, m_renderAPI->GetDrawingBufferHeight());
  }

  void WebGLRenderingContext::DrawingBufferHeightSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::TypeError::New(env, "drawingBufferHeight is readonly.").ThrowAsJavaScriptException();
  }

}
