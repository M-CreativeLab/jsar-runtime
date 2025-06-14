#pragma once

#include "../canvas/image_bitmap.hpp"
#include "../canvas/image_data.hpp"
#include "../canvas/canvas.hpp"
#include "../dom/html_canvas_element.hpp"
#include "../dom/html_image_element.hpp"

#include "./rendering_context.hpp"
#include "./buffer.hpp"
#include "./framebuffer.hpp"
#include "./renderbuffer.hpp"
#include "./placeholders.hpp"
#include "./program.hpp"
#include "./texture.hpp"
#include "./shader.hpp"
#include "./uniform_location.hpp"
#include "./active_info.hpp"

using namespace std;
using namespace node;
using namespace commandbuffers;
using namespace canvasbinding;

namespace webgl
{
  template <typename ObjectType, typename ContextType>
  Napi::Object WebGLBaseRenderingContext<ObjectType, ContextType>::MakeFromHost(Napi::Env env)
  {
    Napi::EscapableHandleScope scope(env);
    WebGLRenderingContextSourceType sourceType = WebGLRenderingContextSourceType::kHost;
    auto typeExternal = Napi::External<uint32_t>::New(env, reinterpret_cast<uint32_t *>(&sourceType));
    if (ObjectType::constructor == nullptr)
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return env.Undefined().ToObject();
    }
    Napi::Object instance = ObjectType::constructor->New({typeExternal});
    return scope.Escape(instance).ToObject();
  }

  template <typename ObjectType, typename ContextType>
  WebGLBaseRenderingContext<ObjectType, ContextType>::WebGLBaseRenderingContext(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<ObjectType>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsExternal())
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }

    auto externalData = info[0].As<Napi::External<uint32_t>>().Data();
    if (externalData == nullptr)
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }

    WebGLRenderingContextSourceType sourceType = static_cast<WebGLRenderingContextSourceType>(*externalData);
    if (sourceType == WebGLRenderingContextSourceType::kHost)
    {
      auto clientContext = TrClientContextPerProcess::Get();
      assert(clientContext != nullptr);
      glContext_ = clientContext->createHostWebGLContext();
      assert(glContext_ != nullptr);
      sourceType_ = WebGLRenderingContextSourceType::kHost;

      /**
       * Write _screenCanvas(ReadOnlyScreenCanvas) property
       *
       * TODO: will change to `.canvas` when DOM implementation is ready.
       * TODO: move to the MakeFromHost() method.
       */
      auto jsThis = info.This().As<Napi::Object>();
      auto canvas = canvasbinding::ReadOnlyScreenCanvas::NewInstance(env,
                                                                     getDrawingBufferWidth(),
                                                                     getDrawingBufferHeight());
      jsThis.Set("_screenCanvas", canvas);
    }
    else if (
      sourceType == WebGLRenderingContextSourceType::kCanvas ||
      sourceType == WebGLRenderingContextSourceType::kOffscreenCanvas)
    {
      Napi::TypeError::New(env, "WebGL context creation has not yet supported from canvas or offscreen canvas.")
        .ThrowAsJavaScriptException();
      return;
    }
    else
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }
  }

  template <typename ObjectType, typename ContextType>
  WebGLBaseRenderingContext<ObjectType, ContextType>::~WebGLBaseRenderingContext()
  {
    if (sourceType_ == WebGLRenderingContextSourceType::kHost)
    {
      auto clientContext = TrClientContextPerProcess::Get();
      assert(clientContext != nullptr);
      clientContext->removeHostWebGLContext(glContext_->id); // Remove the host context from the client context
    }
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::MakeXRCompatible(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    glContext_->makeXRCompatible();
    auto deferred = Napi::Promise::Deferred::New(env);
    deferred.Resolve(env.Undefined());
    return deferred.Promise();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::SetDefaultCoordHandedness(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "setDefaultCoordHandedness() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsString())
    {
      Napi::TypeError::New(env, "setDefaultCoordHandedness() 1st argument must be a string.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto handednessStr = info[0].As<Napi::String>().Utf8Value();
    if (handednessStr == "left")
    {
      glContext_->defaultCoordHandedness = commandbuffers::MatrixHandedness::MATRIX_LEFT_HANDED;
    }
    else if (handednessStr == "right")
    {
      glContext_->defaultCoordHandedness = commandbuffers::MatrixHandedness::MATRIX_RIGHT_HANDED;
    }
    else
    {
      Napi::TypeError::New(env, "setDefaultCoordHandedness() 1st argument must be 'left' or 'right'.")
        .ThrowAsJavaScriptException();
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::IsContextLost(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, glContext_->isContextLost());
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::GetContextAttributes(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto &contextAttributes = glContext_->contextAttributes;
    Napi::Object attribs = Napi::Object::New(env);
    attribs.Set("alpha", Napi::Boolean::New(env, contextAttributes.alpha));
    attribs.Set("antialias", Napi::Boolean::New(env, contextAttributes.antialias));
    attribs.Set("depth", Napi::Boolean::New(env, contextAttributes.depth));
    attribs.Set("stencil", Napi::Boolean::New(env, contextAttributes.stencil));
    attribs.Set("failIfMajorPerformanceCaveat", Napi::Boolean::New(env, contextAttributes.failIfMajorPerformanceCaveat));
    attribs.Set("premultipliedAlpha", Napi::Boolean::New(env, contextAttributes.premultipliedAlpha));
    attribs.Set("preserveDrawingBuffer", Napi::Boolean::New(env, contextAttributes.preserveDrawingBuffer));
    attribs.Set("xrCompatible", Napi::Boolean::New(env, contextAttributes.xrCompatible));
    attribs.Set("powerPreference", Napi::String::New(env, contextAttributes.powerPreference));
    return attribs;
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::CreateProgram(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return WebGLProgram::NewInstance(env, glContext_->createProgram());
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::DeleteProgram(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "deleteProgram() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!WebGLProgram::IsInstanceOf(info[0]))
    {
      Napi::TypeError::New(env, "deleteProgram() 1st argument must be a WebGLProgram.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    glContext_->deleteProgram(program->handle());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::LinkProgram(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "linkProgram() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    // check if the argument is a WebGLProgram type
    if (!WebGLProgram::IsInstanceOf(info[0]))
    {
      Napi::TypeError::New(env, "linkProgram() takes a WebGLProgram as argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    try
    {
      glContext_->linkProgram(program->handle());
    }
    catch (const std::exception &e)
    {
      Napi::TypeError::New(env, e.what()).ThrowAsJavaScriptException();
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::UseProgram(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "useProgram() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!WebGLProgram::IsInstanceOf(info[0]))
    {
      Napi::TypeError::New(env, "useProgram() 1st argument must be a WebGLProgram.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    glContext_->useProgram(program->handle());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::BindAttribLocation(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "bindAttribLocation() takes 3 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!WebGLProgram::IsInstanceOf(info[0]))
    {
      Napi::TypeError::New(env, "bindAttribLocation() 1st argument(program) must be a WebGLProgram.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsNumber())
    {
      Napi::TypeError::New(env, "bindAttribLocation() 2nd argument(index) must be a number.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[2].IsString())
    {
      Napi::TypeError::New(env, "bindAttribLocation() 3rd argument(name) must be a string.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    uint32_t index = info[1].ToNumber().Uint32Value();
    std::string name = info[2].As<Napi::String>().Utf8Value();

    glContext_->bindAttribLocation(program->handle(), index, name);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::GetProgramParameter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "getProgramParameter() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!WebGLProgram::IsInstanceOf(info[0]))
    {
      Napi::TypeError::New(env, "getProgramParameter() 1st argument(program) must be a WebGLProgram.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    int pname = info[1].As<Napi::Number>().Int32Value();

    try
    {
      int v = glContext_->getProgramParameter(program->handle(), pname);
      return Napi::Number::New(env, v);
    }
    catch (const std::exception &e)
    {
      return env.Undefined();
    }
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::GetProgramInfoLog(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "getProgramInfoLog() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!WebGLProgram::IsInstanceOf(info[0]))
    {
      Napi::TypeError::New(env, "getProgramInfoLog() 1st argument(program) must be a WebGLProgram.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    try
    {
      auto log = glContext_->getProgramInfoLog(program->handle());
      return Napi::String::New(env, log);
    }
    catch (const std::exception &e)
    {
      return env.Undefined();
    }
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::AttachShader(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "attachShader() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!WebGLProgram::IsInstanceOf(info[0]))
    {
      Napi::TypeError::New(env, "attachShader() 1st argument(program) must be a WebGLProgram.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!WebGLShader::IsInstanceOf(info[1]))
    {
      Napi::TypeError::New(env, "attachShader() 2nd argument(shader) must be an instance of `WebGLShader`.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    auto shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[1].As<Napi::Object>());

    glContext_->attachShader(program->handle(), shader->handle());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::DetachShader(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "detachShader() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!WebGLProgram::IsInstanceOf(info[0]))
    {
      Napi::TypeError::New(env, "detachShader() 1st argument(program) must be a WebGLProgram.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    auto shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[1].As<Napi::Object>());

    glContext_->detachShader(program->handle(), shader->handle());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::CreateShader(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "createShader() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int typeId = info[0].As<Napi::Number>().Int32Value();
    client_graphics::WebGLShaderType shaderType = static_cast<client_graphics::WebGLShaderType>(typeId);
    return WebGLShader::NewInstance(env, glContext_->createShader(shaderType));
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::DeleteShader(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "deleteShader() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
    glContext_->deleteShader(shader->handle());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::ShaderSource(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "shaderSource() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
    std::string source = info[1].As<Napi::String>().Utf8Value();

    glContext_->shaderSource(shader->handle(), source);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::CompileShader(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "compileShader() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
    glContext_->compileShader(shader->handle());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::GetShaderSource(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "getShaderSource() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
    try
    {
      auto source = glContext_->getShaderSource(shader->handle());
      return Napi::String::New(env, source);
    }
    catch (const std::exception &e)
    {
      return env.Undefined();
    }
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::GetShaderParameter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "getShaderParameter() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
    int pname = info[1].As<Napi::Number>().Int32Value();
    try
    {
      int v = glContext_->getShaderParameter(shader->handle(), pname);
      return Napi::Number::New(env, v);
    }
    catch (const std::exception &e)
    {
      return env.Undefined();
    }
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::GetShaderInfoLog(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "getShaderInfoLog() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
    try
    {
      auto log = glContext_->getShaderInfoLog(shader->handle());
      return Napi::String::New(env, log);
    }
    catch (const std::exception &e)
    {
      return env.Undefined();
    }
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::CreateBuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return WebGLBuffer::NewInstance(env, glContext_->createBuffer());
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::DeleteBuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "deleteBuffer() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto buffer = Napi::ObjectWrap<WebGLBuffer>::Unwrap(info[0].As<Napi::Object>());
    glContext_->deleteBuffer(buffer->handle());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::BindBuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "bindBuffer() takes 2 arguments.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "the first argument(target) should be a number.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int targetInt = info[0].As<Napi::Number>().Int32Value();
    auto target = static_cast<client_graphics::WebGLBufferBindingTarget>(targetInt);
    if (info[1].IsObject())
    {
      auto buffer = Napi::ObjectWrap<WebGLBuffer>::Unwrap(info[1].As<Napi::Object>());
      glContext_->bindBuffer(target, buffer->handle());
    }
    else
    {
      glContext_->bindBuffer(target, nullptr);
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::BufferData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "bufferData() takes 3 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    /**
     * TODO: support bufferData() with size
     */
    if (info[1].IsNumber())
    {
      Napi::TypeError::New(env, "bufferData(target, size, usage) is not supported yet.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto targetInt = info[0].As<Napi::Number>().Int32Value();
    auto target = static_cast<client_graphics::WebGLBufferBindingTarget>(targetInt);
    auto usageInt = info[2].As<Napi::Number>().Int32Value();
    auto usage = static_cast<client_graphics::WebGLBufferUsage>(usageInt);

    auto jsBuffer = info[1];
    void *bufferData = nullptr;
    size_t bufferSize = 0;
    if (jsBuffer.IsDataView() || jsBuffer.IsTypedArray())
    {
      Napi::ArrayBuffer byteBuffer;
      size_t byteLength, byteOffset;
      if (jsBuffer.IsDataView())
      {
        auto dataView = jsBuffer.As<Napi::DataView>();
        byteBuffer = dataView.ArrayBuffer();
        byteLength = dataView.ByteLength();
        byteOffset = dataView.ByteOffset();
      }
      else
      {
        auto typedArray = jsBuffer.As<Napi::TypedArray>();
        byteBuffer = typedArray.ArrayBuffer();
        byteLength = typedArray.ByteLength();
        byteOffset = typedArray.ByteOffset();
      }
      auto buffer = Napi::Uint8Array::New(env, byteLength, byteBuffer, byteOffset);
      bufferData = buffer.Data();
      bufferSize = buffer.ByteLength();
    }
    else if (jsBuffer.IsArray())
    {
      auto valuesArray = jsBuffer.As<Napi::Array>();
      auto float32Array = Napi::Float32Array::New(env, valuesArray.Length(), napi_typedarray_type::napi_float32_array);
      for (uint32_t i = 0; i < valuesArray.Length(); i++)
        float32Array[i] = valuesArray.Get(i).ToNumber().FloatValue();
      bufferData = float32Array.Data();
      bufferSize = float32Array.ByteLength();
    }
    else if (jsBuffer.IsArrayBuffer())
    {
      auto buffer = jsBuffer.As<Napi::ArrayBuffer>();
      bufferData = buffer.Data();
      bufferSize = buffer.ByteLength();
    }
    else
    {
      auto consoleWarn = env.Global().Get("console").As<Napi::Object>().Get("warn").As<Napi::Function>();
      auto msg = "Failed to execute 'bufferData' on 'WebGLRenderingContext': parameter 2 is not of type 'ArrayBufferView'";
      consoleWarn.Call(env.Global(), {Napi::String::New(env, msg), jsBuffer});
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    glContext_->bufferData(target, bufferSize, bufferData, usage);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::BufferSubData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "bufferSubData() takes 3 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[2].IsArrayBuffer() && !info[2].IsTypedArray())
    {
      Napi::TypeError::New(env, "the 3rd argument should be an ArrayBuffer or TypedArray when calling bufferSubData().")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto targetInt = info[0].As<Napi::Number>().Int32Value();
    auto target = static_cast<client_graphics::WebGLBufferBindingTarget>(targetInt);
    auto offset = info[1].As<Napi::Number>().Int32Value();

    auto jsBuffer = info[2];
    void *bufferData = nullptr;
    size_t bufferSize = 0;
    if (jsBuffer.IsDataView() || jsBuffer.IsTypedArray())
    {
      Napi::ArrayBuffer byteBuffer;
      size_t byteLength, byteOffset;
      if (jsBuffer.IsDataView())
      {
        auto dataView = jsBuffer.As<Napi::DataView>();
        byteBuffer = dataView.ArrayBuffer();
        byteLength = dataView.ByteLength();
        byteOffset = dataView.ByteOffset();
      }
      else
      {
        auto typedArray = jsBuffer.As<Napi::TypedArray>();
        byteBuffer = typedArray.ArrayBuffer();
        byteLength = typedArray.ByteLength();
        byteOffset = typedArray.ByteOffset();
      }
      auto buffer = Napi::Uint8Array::New(env, byteLength, byteBuffer, byteOffset);
      bufferData = buffer.Data();
      bufferSize = buffer.ByteLength();
    }
    else if (jsBuffer.IsArrayBuffer())
    {
      auto buffer = jsBuffer.As<Napi::ArrayBuffer>();
      bufferData = buffer.Data();
      bufferSize = buffer.ByteLength();
    }
    else
    {
      Napi::TypeError::New(env, "the 2nd argument(data) should be an ArrayBuffer, TypedArray or DataView in bufferData().")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    glContext_->bufferSubData(target, offset, bufferSize, bufferData);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::CreateFramebuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return WebGLFramebuffer::NewInstance(env, glContext_->createFramebuffer(), false);
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::DeleteFramebuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "deleteFramebuffer() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    else if (
      !info[0].IsObject() ||
      !info[0].As<Napi::Object>().InstanceOf(WebGLFramebuffer::constructor->Value()))
    {
      Napi::TypeError::New(env, "deleteFramebuffer() 1st argument must be a WebGLFramebuffer.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto framebuffer = WebGLFramebuffer::Unwrap(info[0].As<Napi::Object>());
    if (!framebuffer->isHost)
      glContext_->deleteFramebuffer(framebuffer->handle());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::BindFramebuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "bindFramebuffer() must take a target parameter").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber()) // target
    {
      Napi::TypeError::New(env, "the first argument must be a number.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int targetInt = info[0].As<Napi::Number>().Int32Value();
    auto target = static_cast<client_graphics::WebGLFramebufferBindingTarget>(targetInt);
    if (WebGLFramebuffer::IsInstanceOf(info[1]))
    {
      auto framebuffer = Napi::ObjectWrap<WebGLFramebuffer>::Unwrap(info[1].As<Napi::Object>());
      glContext_->bindFramebuffer(target, framebuffer->handle());
    }
    else
    {
      glContext_->bindFramebuffer(target, nullptr);
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::FramebufferRenderbuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 4)
    {
      Napi::TypeError::New(env, "framebufferRenderbuffer() takes 4 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto target = info[0].As<Napi::Number>().Int32Value();
    auto attachment = info[1].As<Napi::Number>().Int32Value();
    auto renderbuffertarget = info[2].As<Napi::Number>().Int32Value();
    auto renderbuffer = Napi::ObjectWrap<WebGLRenderbuffer>::Unwrap(info[3].As<Napi::Object>());

    glContext_->framebufferRenderbuffer(
      static_cast<client_graphics::WebGLFramebufferBindingTarget>(target),
      static_cast<client_graphics::WebGLFramebufferAttachment>(attachment),
      static_cast<client_graphics::WebGLRenderbufferBindingTarget>(renderbuffertarget),
      renderbuffer->handle());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::FramebufferTexture2D(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 5)
    {
      Napi::TypeError::New(env, "framebufferTexture2D() takes 5 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto jsTexture = info[3];
    if (!jsTexture.IsObject() || WebGLTexture::IsInstanceOf(jsTexture))
    {
      glContext_->setError("framebufferTexture2d", client_graphics::WebGLError::kInvalidOperation, "texture isn't 0 or the name of an existing texture object");
      return env.Undefined();
    }

    auto target = info[0].As<Napi::Number>().Int32Value();
    auto attachment = info[1].As<Napi::Number>().Int32Value();
    auto textarget = info[2].As<Napi::Number>().Int32Value();
    auto texture = Napi::ObjectWrap<WebGLTexture>::Unwrap(jsTexture.As<Napi::Object>());
    auto level = info[4].As<Napi::Number>().Int32Value();

    if (target != WEBGL_FRAMEBUFFER)
    {
      glContext_->setError("framebufferTexture2d", client_graphics::WebGLError::kInvalidEnum, "target must be FRAMEBUFFER");
      return env.Undefined();
    }
    if (textarget != WEBGL_TEXTURE_2D &&
        textarget != WEBGL_TEXTURE_CUBE_MAP_POSITIVE_X &&
        textarget != WEBGL_TEXTURE_CUBE_MAP_NEGATIVE_X &&
        textarget != WEBGL_TEXTURE_CUBE_MAP_POSITIVE_Y &&
        textarget != WEBGL_TEXTURE_CUBE_MAP_NEGATIVE_Y &&
        textarget != WEBGL_TEXTURE_CUBE_MAP_POSITIVE_Z &&
        textarget != WEBGL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
    {
      glContext_->setError("framebufferTexture2d", client_graphics::WebGLError::kInvalidEnum, "textarget must be TEXTURE_2D or one of the TEXTURE_CUBE_MAP_* targets");
      return env.Undefined();
    }

    glContext_->framebufferTexture2D(
      static_cast<client_graphics::WebGLFramebufferBindingTarget>(target),
      static_cast<client_graphics::WebGLFramebufferAttachment>(attachment),
      static_cast<client_graphics::WebGLTexture2DTarget>(textarget),
      texture->handle(),
      level);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::CheckFramebufferStatus(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "checkFramebufferStatus() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int target = info[0].As<Napi::Number>().Int32Value();
    try
    {
      auto r = glContext_->checkFramebufferStatus(static_cast<client_graphics::WebGLFramebufferBindingTarget>(target));
      return Napi::Number::New(env, r);
    }
    catch (const std::exception &e)
    {
      return env.Undefined();
    }
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::CreateRenderbuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return WebGLRenderbuffer::NewInstance(env, glContext_->createRenderbuffer());
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::DeleteRenderbuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !WebGLRenderbuffer::IsInstanceOf(info[0]))
    {
      Napi::TypeError::New(env, "deleteRenderbuffer() takes 1 argument and it must be a WebGLRenderbuffer.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto renderbuffer = Napi::ObjectWrap<WebGLRenderbuffer>::Unwrap(info[0].As<Napi::Object>());
    glContext_->deleteRenderbuffer(renderbuffer->handle());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::BindRenderbuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "bindRenderbuffer() must take a target parameter.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "the first argument should be a number when calling bindRenderbuffer().")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto targetInt = info[0].As<Napi::Number>().Int32Value();
    auto target = static_cast<client_graphics::WebGLRenderbufferBindingTarget>(targetInt);
    if (WebGLRenderbuffer::IsInstanceOf(info[1]))
    {
      auto renderbuffer = Napi::ObjectWrap<WebGLRenderbuffer>::Unwrap(info[1].As<Napi::Object>());
      glContext_->bindRenderbuffer(target, renderbuffer->handle());
    }
    else
    {
      glContext_->bindRenderbuffer(target, nullptr);
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::RenderbufferStorage(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 4)
    {
      Napi::TypeError::New(env, "renderbufferStorage() takes 4 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto target = info[0].As<Napi::Number>().Int32Value();
    auto internalformat = info[1].As<Napi::Number>().Int32Value();
    auto width = info[2].As<Napi::Number>().Int32Value();
    auto height = info[3].As<Napi::Number>().Int32Value();

    glContext_->renderbufferStorage(static_cast<client_graphics::WebGLRenderbufferBindingTarget>(target),
                                    internalformat,
                                    width,
                                    height);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::CreateTexture(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return WebGLTexture::NewInstance(env, glContext_->createTexture());
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::DeleteTexture(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "deleteTexture() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || WebGLTexture::IsInstanceOf(info[0]))
    {
      Napi::TypeError::New(env, "deleteTexture() takes a WebGLTexture as argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto texture = Napi::ObjectWrap<WebGLTexture>::Unwrap(info[0].As<Napi::Object>());
    glContext_->deleteTexture(texture->handle());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::BindTexture(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "bindTexture() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "the target to bindTexture() is invalid.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto targetInt = info[0].As<Napi::Number>().Int32Value();
    auto target = static_cast<client_graphics::WebGLTextureTarget>(targetInt);

    if (WebGLTexture::IsInstanceOf(info[1]))
    {
      auto texture = Napi::ObjectWrap<WebGLTexture>::Unwrap(info[1].As<Napi::Object>());
      glContext_->bindTexture(target, texture->handle());
    }
    else if (info[1].IsNull() || info[1].IsUndefined())
    {
      glContext_->bindTexture(target, nullptr);
    }
    else
    {
      Napi::TypeError::New(env, "the texture to bindTexture() is invalid, must be null or a WebGLTexture object.")
        .ThrowAsJavaScriptException();
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::TexImage2D(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 6)
    {
      Napi::TypeError::New(env, "texImage2D() takes at least 6 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto target = info[0].As<Napi::Number>().Uint32Value();
    auto level = info[1].As<Napi::Number>().Uint32Value();
    auto internalformat = info[2].As<Napi::Number>().Uint32Value();

    int format, width, height, border = 0;
    int pixelType;
    unsigned char *pixelsData = nullptr;

    SkPixmap imagePixmap; // for image source pixels
    if (info.Length() == 6)
    {
      format = info[3].As<Napi::Number>().Uint32Value();
      pixelType = info[4].As<Napi::Number>().Uint32Value();
      if (!info[5].IsObject())
      {
        Napi::TypeError::New(env, "the image source should be an object.").ThrowAsJavaScriptException();
        return env.Undefined();
      }

      shared_ptr<canvas::ImageSource> imageSource = getImageSourceFromObject(env, info[5].ToObject());
      if (imageSource == nullptr)
      {
        auto msg =
          "Failed to execute 'texImage2D' on 'WebGLRenderingContext': parameter 5 is not of type "
          "'ImageBitmap', 'ImageData', 'OffscreenCanvas', 'HTMLCanvasElement', or 'HTMLImageElement'";
        Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
        return env.Undefined();
      }

      if (!imageSource->readPixels(imagePixmap))
      {
        Napi::TypeError::New(env, "Failed to read pixels from image source.")
          .ThrowAsJavaScriptException();
        return env.Undefined();
      }

      width = imagePixmap.width();
      height = imagePixmap.height();
      format = WEBGL_RGBA;
      internalformat = WEBGL2_RGBA8;
      pixelsData = reinterpret_cast<unsigned char *>(imagePixmap.writable_addr());
    }
    else if (info.Length() == 9)
    {
      if (!info[3].IsNumber() || !info[4].IsNumber())
      {
        Napi::Object debugObject = Napi::Object::New(env);
        debugObject.Set("width", info[3]);
        debugObject.Set("height", info[4]);
        env.Global().Get("console").As<Napi::Object>().Get("log").As<Napi::Function>()({debugObject});
        Napi::TypeError::New(env, "width or height is not a number.")
          .ThrowAsJavaScriptException();
        return env.Undefined();
      }

      width = info[3].As<Napi::Number>().Uint32Value();
      height = info[4].As<Napi::Number>().Uint32Value();
      border = info[5].As<Napi::Number>().Uint32Value();
      format = info[6].As<Napi::Number>().Uint32Value();
      pixelType = info[7].As<Napi::Number>().Uint32Value();

      Napi::Value pixelsValue = info[8];
      if (pixelsValue.IsNull() || pixelsValue.IsUndefined())
      {
        pixelsData = nullptr;
      }
      else if (pixelsValue.IsTypedArray() || pixelsValue.IsArrayBuffer())
      {
        if (pixelsValue.IsArrayBuffer())
        {
          auto arraybuffer = pixelsValue.As<Napi::ArrayBuffer>();
          pixelsData = static_cast<unsigned char *>(arraybuffer.Data());
        }
        else if (pixelsValue.IsTypedArray())
        {
          auto typedarray = pixelsValue.As<Napi::TypedArray>();
          pixelsData = static_cast<unsigned char *>(typedarray.ArrayBuffer().Data()) + typedarray.ByteOffset();
        }
      }
      else
      {
        Napi::TypeError::New(env, "The pixels should be a TypedArray or ArrayBuffer.").ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }
    else
    {
      Napi::TypeError::New(env, "Invalid number of arguments for texImage2D().").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    glContext_->texImage2D(
      static_cast<client_graphics::WebGLTexture2DTarget>(target),
      level,
      internalformat,
      width,
      height,
      border,
      static_cast<client_graphics::WebGLTextureFormat>(format),
      static_cast<client_graphics::WebGLPixelType>(pixelType),
      pixelsData);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::TexSubImage2D(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 7)
    {
      Napi::TypeError::New(env, "texSubImage2D() takes at least 7 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto target = info[0].ToNumber().Int32Value();
    auto level = info[1].ToNumber().Int32Value();
    auto xoffset = info[2].ToNumber().Int32Value();
    auto yoffset = info[3].ToNumber().Int32Value();

    int format, width, height;
    int pixelType;
    unsigned char *pixelsData = nullptr;

    SkPixmap imagePixmap; // for image source pixels

    if (info.Length() == 7)
    {
      format = info[4].ToNumber().Int32Value();
      pixelType = info[5].ToNumber().Int32Value();

      auto imageSourceValue = info[6];
      if (!imageSourceValue.IsObject())
      {
        Napi::TypeError::New(env, "the image source should be an object.").ThrowAsJavaScriptException();
        return env.Undefined();
      }

      shared_ptr<canvas::ImageSource> imageSource = getImageSourceFromObject(env, imageSourceValue.ToObject());
      if (imageSource == nullptr)
      {
        auto msg =
          "Failed to execute 'texSubImage2D' on 'WebGLRenderingContext': parameter 6 is not of type "
          "'ImageBitmap', 'ImageData', 'OffscreenCanvas', 'HTMLCanvasElement', or 'HTMLImageElement'";
        Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
        return env.Undefined();
      }

      if (!imageSource->readPixels(imagePixmap))
      {
        Napi::TypeError::New(env, "Failed to call `texSubImage2D`: unable to read pixels from image source.")
          .ThrowAsJavaScriptException();
        return env.Undefined();
      }

      width = imagePixmap.width();
      height = imagePixmap.height();
      pixelsData = reinterpret_cast<unsigned char *>(imagePixmap.writable_addr());
    }
    else if (info.Length() >= 8)
    {
      width = info[4].ToNumber().Int32Value();
      height = info[5].ToNumber().Int32Value();
      format = info[6].ToNumber().Int32Value();
      pixelType = info[7].ToNumber().Int32Value();

      if (info.Length() >= 9)
      {
        auto pixelsValue = info[8];
        if (pixelsValue.IsNull() || pixelsValue.IsUndefined())
        {
          pixelsData = nullptr;
        }
        else if (pixelsValue.IsTypedArray())
        {
          auto typedarray = pixelsValue.As<Napi::TypedArray>();
          pixelsData = static_cast<unsigned char *>(typedarray.ArrayBuffer().Data()) + typedarray.ByteOffset();
        }
        else
        {
          Napi::TypeError::New(env, "The pixels should be a TypedArray(ArrayBufferView).").ThrowAsJavaScriptException();
          return env.Undefined();
        }
      }
    }
    else
    {
      Napi::TypeError::New(env, "Invalid number of arguments for texSubImage2D().").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    glContext_->texSubImage2D(
      static_cast<client_graphics::WebGLTexture2DTarget>(target),
      level,
      xoffset,
      yoffset,
      width,
      height,
      static_cast<client_graphics::WebGLTextureFormat>(format),
      static_cast<client_graphics::WebGLPixelType>(pixelType),
      pixelsData);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::CopyTexImage2D(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int target = info[0].As<Napi::Number>().Int32Value();
    int level = info[1].As<Napi::Number>().Int32Value();
    int internalformat = info[2].As<Napi::Number>().Int32Value();
    int x = info[3].As<Napi::Number>().Int32Value();
    int y = info[4].As<Napi::Number>().Int32Value();
    int width = info[5].As<Napi::Number>().Int32Value();
    int height = info[6].As<Napi::Number>().Int32Value();
    int border = info[7].As<Napi::Number>().Int32Value();

    glContext_->copyTexImage2D(
      static_cast<client_graphics::WebGLTexture2DTarget>(target),
      level,
      internalformat,
      x,
      y,
      width,
      height,
      border);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::CopyTexSubImage2D(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int target = info[0].As<Napi::Number>().Int32Value();
    int level = info[1].As<Napi::Number>().Int32Value();
    int xoffset = info[2].As<Napi::Number>().Int32Value();
    int yoffset = info[3].As<Napi::Number>().Int32Value();
    int x = info[4].As<Napi::Number>().Int32Value();
    int y = info[5].As<Napi::Number>().Int32Value();
    int width = info[6].As<Napi::Number>().Int32Value();
    int height = info[7].As<Napi::Number>().Int32Value();

    glContext_->copyTexSubImage2D(
      static_cast<client_graphics::WebGLTexture2DTarget>(target),
      level,
      xoffset,
      yoffset,
      x,
      y,
      width,
      height);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::TexParameteri(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int target = info[0].As<Napi::Number>().Int32Value();
    int pname = info[1].As<Napi::Number>().Int32Value();
    int param = info[2].As<Napi::Number>().Int32Value();

    glContext_->texParameteri(
      static_cast<client_graphics::WebGLTextureTarget>(target),
      static_cast<client_graphics::WebGLTextureParameterName>(pname),
      param);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::TexParameterf(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int target = info[0].As<Napi::Number>().Int32Value();
    int pname = info[1].As<Napi::Number>().Int32Value();
    float param = info[2].As<Napi::Number>().FloatValue();

    glContext_->texParameterf(
      static_cast<client_graphics::WebGLTextureTarget>(target),
      static_cast<client_graphics::WebGLTextureParameterName>(pname),
      param);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::ActiveTexture(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "activeTexture() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "activeTexture() 1st argument(texture) must be a number.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int unit = info[0].As<Napi::Number>().Int32Value();
    glContext_->activeTexture(static_cast<client_graphics::WebGLTextureUnit>(unit));
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::GenerateMipmap(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int target = info[0].As<Napi::Number>().Int32Value();
    glContext_->generateMipmap(static_cast<client_graphics::WebGLTextureTarget>(target));
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::EnableVertexAttribArray(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "enableVertexAttribArray() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto index = info[0].As<Napi::Number>().Uint32Value();
    glContext_->enableVertexAttribArray(index);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::DisableVertexAttribArray(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "disableVertexAttribArray() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto index = info[0].As<Napi::Number>().Uint32Value();
    glContext_->disableVertexAttribArray(index);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::VertexAttribPointer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 6)
    {
      Napi::TypeError::New(env, "vertexAttribPointer() takes 6 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto index = info[0].As<Napi::Number>().Uint32Value();
    auto size = info[1].As<Napi::Number>().Uint32Value();
    auto type = info[2].As<Napi::Number>().Uint32Value();
    auto normalized = info[3].As<Napi::Boolean>().Value();
    auto stride = info[4].As<Napi::Number>().Uint32Value();
    auto offset = info[5].As<Napi::Number>().Uint32Value();

    glContext_->vertexAttribPointer(index, size, type, normalized, stride, offset);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::GetActiveAttrib(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "getActiveAttrib() requires 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!WebGLProgram::IsInstanceOf(info[0]))
    {
      Napi::TypeError::New(env, "getActiveAttrib() 1st argument(program) must be a WebGLProgram.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    uint32_t index = 0;
    if (info[1].IsNumber())
      index = info[1].As<Napi::Number>().Uint32Value();

    auto attrib = glContext_->getActiveAttrib(program->handle(), index);
    if (!attrib.has_value())
      return env.Null();
    return WebGLActiveInfo::NewInstance(env, attrib.value());
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::GetActiveUniform(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "getActiveUniform() requires 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!WebGLProgram::IsInstanceOf(info[0]))
    {
      Napi::TypeError::New(env, "getActiveUniform() 1st argument(program) must be a WebGLProgram.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    int index = 0;
    if (info[1].IsNumber())
      index = info[1].As<Napi::Number>().Int32Value();

    auto uniform = glContext_->getActiveUniform(program->handle(), index);
    if (!uniform.has_value())
      return env.Null();
    return WebGLActiveInfo::NewInstance(env, uniform.value());
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::GetAttribLocation(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "getAttribLocation() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!WebGLProgram::IsInstanceOf(info[0]))
    {
      Napi::TypeError::New(env, "getAttribLocation() 1st argument(program) must be a WebGLProgram.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    std::string name = info[1].As<Napi::String>().Utf8Value();
    auto loc = glContext_->getAttribLocation(program->handle(), name);
    if (loc.has_value())
      return Napi::Number::New(env, loc.value());
    else
      return Napi::Number::New(env, -1);
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::GetUniformLocation(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "getUniformLocation() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!WebGLProgram::IsInstanceOf(info[0]))
    {
      Napi::TypeError::New(env, "getUniformLocation() 1st argument(program) must be a WebGLProgram.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    std::string name = info[1].As<Napi::String>().Utf8Value();

    auto uniformLoc = glContext_->getUniformLocation(program->handle(), name);
    if (!uniformLoc.has_value())
      return env.Null();
    return WebGLUniformLocation::NewInstance(env, uniformLoc.value());
  }

#define UNIFORM1X_IMPL(X, JS_VALUE_GETTER)                                                                         \
  template <typename ObjectType, typename ContextType>                                                             \
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::Uniform1##X(const Napi::CallbackInfo &info)      \
  {                                                                                                                \
    Napi::Env env = info.Env();                                                                                    \
    Napi::HandleScope scope(env);                                                                                  \
                                                                                                                   \
    if (info.Length() < 2)                                                                                         \
    {                                                                                                              \
      Napi::TypeError::New(env, "uniform1" #X "() takes 2 arguments.").ThrowAsJavaScriptException();               \
      return env.Undefined();                                                                                      \
    }                                                                                                              \
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value())) \
    {                                                                                                              \
      Napi::TypeError::New(env, "uniform1" #X "() 1st argument(program) must be a WebGLUniformLocation object.")   \
        .ThrowAsJavaScriptException();                                                                             \
      return env.Undefined();                                                                                      \
    }                                                                                                              \
                                                                                                                   \
    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());                    \
    auto x = info[1].ToNumber().JS_VALUE_GETTER();                                                                 \
                                                                                                                   \
    glContext_->uniform1##X(location->handle(), x);                                                                \
    return env.Undefined();                                                                                        \
  }

#define UNIFORM2X_IMPL(X, JS_VALUE_GETTER)                                                                         \
  template <typename ObjectType, typename ContextType>                                                             \
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::Uniform2##X(const Napi::CallbackInfo &info)      \
  {                                                                                                                \
    Napi::Env env = info.Env();                                                                                    \
    Napi::HandleScope scope(env);                                                                                  \
                                                                                                                   \
    if (info.Length() < 3)                                                                                         \
    {                                                                                                              \
      Napi::TypeError::New(env, "uniform2" #X "() takes 3 arguments.").ThrowAsJavaScriptException();               \
      return env.Undefined();                                                                                      \
    }                                                                                                              \
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value())) \
    {                                                                                                              \
      Napi::TypeError::New(env, "uniform2" #X "() 1st argument(program) must be a WebGLUniformLocation object.")   \
        .ThrowAsJavaScriptException();                                                                             \
      return env.Undefined();                                                                                      \
    }                                                                                                              \
                                                                                                                   \
    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());                    \
    auto x = info[1].ToNumber().JS_VALUE_GETTER();                                                                 \
    auto y = info[2].ToNumber().JS_VALUE_GETTER();                                                                 \
                                                                                                                   \
    glContext_->uniform2##X(location->handle(), x, y);                                                             \
    return env.Undefined();                                                                                        \
  }

#define UNIFORM3X_IMPL(X, JS_VALUE_GETTER)                                                                         \
  template <typename ObjectType, typename ContextType>                                                             \
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::Uniform3##X(const Napi::CallbackInfo &info)      \
  {                                                                                                                \
    Napi::Env env = info.Env();                                                                                    \
    Napi::HandleScope scope(env);                                                                                  \
                                                                                                                   \
    if (info.Length() < 4)                                                                                         \
    {                                                                                                              \
      Napi::TypeError::New(env, "uniform3" #X "() takes 4 arguments.").ThrowAsJavaScriptException();               \
      return env.Undefined();                                                                                      \
    }                                                                                                              \
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value())) \
    {                                                                                                              \
      Napi::TypeError::New(env, "uniform3" #X "() 1st argument(program) must be a WebGLUniformLocation object.")   \
        .ThrowAsJavaScriptException();                                                                             \
      return env.Undefined();                                                                                      \
    }                                                                                                              \
                                                                                                                   \
    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());                    \
    auto x = info[1].ToNumber().JS_VALUE_GETTER();                                                                 \
    auto y = info[2].ToNumber().JS_VALUE_GETTER();                                                                 \
    auto z = info[3].ToNumber().JS_VALUE_GETTER();                                                                 \
                                                                                                                   \
    glContext_->uniform3##X(location->handle(), x, y, z);                                                          \
    return env.Undefined();                                                                                        \
  }

#define UNIFORM4X_IMPL(X, JS_VALUE_GETTER)                                                                         \
  template <typename ObjectType, typename ContextType>                                                             \
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::Uniform4##X(const Napi::CallbackInfo &info)      \
  {                                                                                                                \
    Napi::Env env = info.Env();                                                                                    \
    Napi::HandleScope scope(env);                                                                                  \
                                                                                                                   \
    if (info.Length() < 5)                                                                                         \
    {                                                                                                              \
      Napi::TypeError::New(env, "uniform4" #X "() takes 5 arguments.").ThrowAsJavaScriptException();               \
      return env.Undefined();                                                                                      \
    }                                                                                                              \
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value())) \
    {                                                                                                              \
      Napi::TypeError::New(env, "uniform4" #X "() 1st argument(program) must be a WebGLUniformLocation object.")   \
        .ThrowAsJavaScriptException();                                                                             \
      return env.Undefined();                                                                                      \
    }                                                                                                              \
                                                                                                                   \
    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());                    \
    auto x = info[1].ToNumber().JS_VALUE_GETTER();                                                                 \
    auto y = info[2].ToNumber().JS_VALUE_GETTER();                                                                 \
    auto z = info[3].ToNumber().JS_VALUE_GETTER();                                                                 \
    auto w = info[4].ToNumber().JS_VALUE_GETTER();                                                                 \
                                                                                                                   \
    glContext_->uniform4##X(location->handle(), x, y, z, w);                                                       \
    return env.Undefined();                                                                                        \
  }

  UNIFORM1X_IMPL(f, FloatValue)
  UNIFORM1X_IMPL(i, Int32Value)

  UNIFORM2X_IMPL(f, FloatValue)
  UNIFORM2X_IMPL(i, Int32Value)

  UNIFORM3X_IMPL(f, FloatValue)
  UNIFORM3X_IMPL(i, Int32Value)

  UNIFORM4X_IMPL(f, FloatValue)
  UNIFORM4X_IMPL(i, Int32Value)

  /**
   * This function is used in uniform*v() functions to get a vector of values from the 2nd argument: `Array`, `Int32Array`
   * and `Float32Array`.
   *
   * @param env The environment that the Node.js addon is running in.
   * @param jsValues The 2nd argument of uniform*v() functions.
   * @param valueTypedArrayType The TypedArray type of the values.
   * @param n The number of values that should be taken from the 2nd argument.
   * @returns A vector of values.
   */
  template <typename ValueType>
  vector<ValueType> getUniformValuesVector(
    Napi::Env env,
    Napi::Value jsValues,
    napi_typedarray_type valueTypedArrayType,
    size_t n)
  {
    if (valueTypedArrayType != napi_float32_array && valueTypedArrayType != napi_int32_array)
      throw WebGLUniformError("TypedArray type should be either napi_float32_array or napi_int32_array.");

    size_t valuesCount;
    if (jsValues.IsArray())
      valuesCount = jsValues.As<Napi::Array>().Length();
    else if (jsValues.IsTypedArray())
    {
      auto typedArray = jsValues.As<Napi::TypedArray>();
      if (typedArray.TypedArrayType() != valueTypedArrayType)
        throw WebGLUniformError("value must be correct TypedArray type.");
      valuesCount = typedArray.ElementLength();
    }
    else
      throw WebGLUniformError("value must be a float array or Float32Array.");

    if (valuesCount < n)
    {
      auto msg = "should take at least " + std::to_string(n) + " values";
      throw WebGLUniformError(msg);
    }

    std::vector<ValueType> data(valuesCount);
    if (jsValues.IsArray())
    {
      auto array = jsValues.As<Napi::Array>();
      for (size_t i = 0; i < valuesCount; i++)
        data[i] = array.Get(i).ToNumber();
    }
    else
    {
      if (valueTypedArrayType == napi_float32_array)
      {
        auto array = jsValues.As<Napi::Float32Array>();
        for (size_t i = 0; i < valuesCount; i++)
          data[i] = array.Get(i).ToNumber().FloatValue();
      }
      else if (valueTypedArrayType == napi_int32_array)
      {
        auto array = jsValues.As<Napi::Int32Array>();
        for (size_t i = 0; i < valuesCount; i++)
          data[i] = array.Get(i).ToNumber().Int32Value();
      }
    }
    return data;
  }

#define UNIFORMNXV_IMPL(N, X, NAPI_VALUE_TYPE, VALUE_TYPE)                                                          \
  template <typename ObjectType, typename ContextType>                                                              \
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::Uniform##N##X##v(const Napi::CallbackInfo &info)  \
  {                                                                                                                 \
    Napi::Env env = info.Env();                                                                                     \
    Napi::HandleScope scope(env);                                                                                   \
                                                                                                                    \
    if (info.Length() < 2)                                                                                          \
    {                                                                                                               \
      Napi::TypeError::New(env, "uniform" #N #X "v() takes 2 arguments.").ThrowAsJavaScriptException();             \
      return env.Undefined();                                                                                       \
    }                                                                                                               \
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))  \
    {                                                                                                               \
      Napi::TypeError::New(env, "uniform" #N #X "v() 1st argument(program) must be a WebGLUniformLocation object.") \
        .ThrowAsJavaScriptException();                                                                              \
      return env.Undefined();                                                                                       \
    }                                                                                                               \
                                                                                                                    \
    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());                     \
    try                                                                                                             \
    {                                                                                                               \
      auto data = getUniformValuesVector<VALUE_TYPE>(env, info[1], NAPI_VALUE_TYPE, N);                             \
      glContext_->uniform##N##X##v(location->handle(), data);                                                       \
    }                                                                                                               \
    catch (WebGLUniformError & e)                                                                                   \
    {                                                                                                               \
      Napi::TypeError::New(env, e.message("uniform" #N #X "v")).ThrowAsJavaScriptException();                       \
    }                                                                                                               \
    return env.Undefined();                                                                                         \
  }

  UNIFORMNXV_IMPL(1, f, napi_float32_array, float)
  UNIFORMNXV_IMPL(1, i, napi_int32_array, int32_t)
  UNIFORMNXV_IMPL(2, f, napi_float32_array, float)
  UNIFORMNXV_IMPL(2, i, napi_int32_array, int32_t)
  UNIFORMNXV_IMPL(3, f, napi_float32_array, float)
  UNIFORMNXV_IMPL(3, i, napi_int32_array, int32_t)
  UNIFORMNXV_IMPL(4, f, napi_float32_array, float)
  UNIFORMNXV_IMPL(4, i, napi_int32_array, int32_t)

#define UNIFORM_MATRIX_NFV_IMPL(N)                                                                                      \
  template <typename ObjectType, typename ContextType>                                                                  \
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::UniformMatrix##N##fv(const Napi::CallbackInfo &info)  \
  {                                                                                                                     \
    Napi::Env env = info.Env();                                                                                         \
    Napi::HandleScope scope(env);                                                                                       \
    if (info.Length() < 3)                                                                                              \
    {                                                                                                                   \
      Napi::TypeError::New(env, "uniformMatrix" #N "fv() takes 3 arguments.").ThrowAsJavaScriptException();             \
      return env.Undefined();                                                                                           \
    }                                                                                                                   \
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))      \
    {                                                                                                                   \
      Napi::TypeError::New(env, "uniformMatrix" #N "fv() 1st argument(program) must be a WebGLUniformLocation object.") \
        .ThrowAsJavaScriptException();                                                                                  \
      return env.Undefined();                                                                                           \
    }                                                                                                                   \
    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());                         \
    bool transpose = info[1].As<Napi::Boolean>().Value();                                                               \
    Napi::Float32Array matricesArray = info[2].As<Napi::Float32Array>();                                                \
                                                                                                                        \
    if (N == 4 && matricesArray.Has(WEBGL_PLACEHOLDERS_PLACEHOLDER_ID_KEY))                                             \
    {                                                                                                                   \
      auto placeholderIdValue = matricesArray.Get(WEBGL_PLACEHOLDERS_PLACEHOLDER_ID_KEY);                               \
      if (!placeholderIdValue.IsNumber())                                                                               \
      {                                                                                                                 \
        Napi::TypeError::New(env, "placeholderId must be a number.").ThrowAsJavaScriptException();                      \
        return env.Undefined();                                                                                         \
      }                                                                                                                 \
      auto placeholderId = (WebGLMatrixPlaceholderId)placeholderIdValue.ToNumber().Uint32Value();                       \
      auto handedness = matricesArray.Get(WEBGL_PLACEHOLDERS_USE_RIGHTHANDED_KEY).ToBoolean()                           \
                          ? MatrixHandedness::MATRIX_RIGHT_HANDED                                                       \
                          : MatrixHandedness::MATRIX_LEFT_HANDED;                                                       \
      MatrixComputationGraph computationGraph(placeholderId, handedness);                                               \
                                                                                                                        \
      if (matricesArray.Has(WEBGL_PLACEHOLDERS_INVERSE_MATRIX_KEY))                                                     \
        computationGraph.inverseMatrix = matricesArray.Get(WEBGL_PLACEHOLDERS_INVERSE_MATRIX_KEY).ToBoolean().Value();  \
      glContext_->uniformMatrix##N##fv(location->handle(), transpose, computationGraph);                                \
      return env.Undefined();                                                                                           \
    }                                                                                                                   \
                                                                                                                        \
    size_t length = matricesArray.ElementLength();                                                                      \
    if (length % (N * N) != 0)                                                                                          \
    {                                                                                                                   \
      Napi::TypeError::New(env, "uniformMatrix" #N "fv() takes " #N "x" #N " float elements array.")                    \
        .ThrowAsJavaScriptException();                                                                                  \
      return env.Undefined();                                                                                           \
    }                                                                                                                   \
    std::vector<float> values(length);                                                                                  \
    for (size_t i = 0; i < length; i++)                                                                                 \
      values[i] = matricesArray.Get(i).ToNumber().FloatValue();                                                         \
    glContext_->uniformMatrix##N##fv(location->handle(), transpose, values);                                            \
    return env.Undefined();                                                                                             \
  }

  UNIFORM_MATRIX_NFV_IMPL(2)
  UNIFORM_MATRIX_NFV_IMPL(3)
  UNIFORM_MATRIX_NFV_IMPL(4)

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::DrawArrays(const Napi::CallbackInfo &info)
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

    glContext_->drawArrays(static_cast<client_graphics::WebGLDrawMode>(mode), first, count);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::DrawElements(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 4)
    {
      Napi::TypeError::New(env, "drawElements() takes 4 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int mode = info[0].As<Napi::Number>().Int32Value();
    int count = info[1].As<Napi::Number>().Int32Value();
    int type = info[2].As<Napi::Number>().Int32Value();
    int offset = info[3].As<Napi::Number>().Int32Value();

    glContext_->drawElements(static_cast<client_graphics::WebGLDrawMode>(mode), count, type, offset);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::Hint(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2)
    {
      Napi::TypeError::New(env, "hint() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int target = info[0].As<Napi::Number>().Int32Value();
    int mode = info[1].As<Napi::Number>().Int32Value();

    glContext_->hint(static_cast<client_graphics::WebGLHintTargetBehavior>(target),
                     static_cast<client_graphics::WebGLHintBehaviorMode>(mode));
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::LineWidth(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1)
    {
      Napi::TypeError::New(env, "lineWidth() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    float width = info[0].As<Napi::Number>().FloatValue();
    glContext_->lineWidth(width);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::PixelStorei(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "pixelStorei() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "pixelStorei() 1st argument(pname) must be a number")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int pname = info[0].As<Napi::Number>().Int32Value();
    int param = info[1].ToNumber().Int32Value();
    glContext_->pixelStorei(static_cast<client_graphics::WebGLPixelStorageParameterName>(pname), param);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::PolygonOffset(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "polygonOffset() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    float factor = info[0].As<Napi::Number>().FloatValue();
    float units = info[1].As<Napi::Number>().FloatValue();

    glContext_->polygonOffset(factor, units);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::Viewport(const Napi::CallbackInfo &info)
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

    try
    {
      glContext_->viewport(x, y, width, height);
    }
    catch (const std::exception &e)
    {
      Napi::TypeError::New(env, e.what()).ThrowAsJavaScriptException();
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::Scissor(const Napi::CallbackInfo &info)
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

    glContext_->scissor(x, y, width, height);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::ClearColor(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 0.0f;

    if (info[0].IsNumber())
      r = info[0].ToNumber().FloatValue();
    if (info[1].IsNumber())
      g = info[1].ToNumber().FloatValue();
    if (info[2].IsNumber())
      b = info[2].ToNumber().FloatValue();
    if (info[3].IsNumber())
      a = info[3].ToNumber().FloatValue();

    glContext_->clearColor(r, g, b, a);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::ClearDepth(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1 && info[0].IsNumber())
      glContext_->clearDepth(info[0].ToNumber().FloatValue());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::ClearStencil(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1 && info[0].IsNumber())
      glContext_->clearStencil(info[0].ToNumber().Int32Value());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::Clear(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1 && info[0].IsNumber())
    {
      uint32_t mask = info[0].ToNumber().Uint32Value();
      glContext_->clear(info[0].ToNumber().Uint32Value());
    }
    else
    {
      glContext_->clear(WEBGL_COLOR_BUFFER_BIT | WEBGL_DEPTH_BUFFER_BIT | WEBGL_STENCIL_BUFFER_BIT);
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::DepthMask(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1 && info[0].IsBoolean())
      glContext_->depthMask(info[0].ToBoolean().Value());
    else
      glContext_->depthMask(false);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::DepthFunc(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1 && info[0].IsNumber())
    {
      uint32_t func = info[0].ToNumber().Uint32Value();
      glContext_->depthFunc(info[0].ToNumber().Int32Value());
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::DepthRange(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 2)
    {
      float near = info[0].ToNumber().FloatValue();
      float far = info[1].ToNumber().FloatValue();
      glContext_->depthRange(near, far);
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::StencilFunc(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 3)
    {
      auto func = info[0].ToNumber().Int32Value();
      auto ref = info[1].ToNumber().Int32Value();
      auto mask = info[2].ToNumber().Uint32Value();
      glContext_->stencilFunc(func, ref, mask);
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::StencilFuncSeparate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 4)
    {
      auto face = info[0].ToNumber().Int32Value();
      auto func = info[1].ToNumber().Int32Value();
      auto ref = info[2].ToNumber().Int32Value();
      auto mask = info[3].ToNumber().Uint32Value();
      glContext_->stencilFuncSeparate(face, func, ref, mask);
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::StencilMask(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1 && info[0].IsNumber())
      glContext_->stencilMask(info[0].ToNumber().Uint32Value());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::StencilMaskSeparate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 2)
    {
      auto face = info[0].ToNumber().Int32Value();
      auto mask = info[1].ToNumber().Uint32Value();
      glContext_->stencilMaskSeparate(face, mask);
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::StencilOp(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 3)
    {
      auto fail = info[0].ToNumber().Int32Value();
      auto zfail = info[1].ToNumber().Int32Value();
      auto zpass = info[2].ToNumber().Int32Value();
      glContext_->stencilOp(fail, zfail, zpass);
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::StencilOpSeparate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 4)
    {
      auto face = info[0].ToNumber().Int32Value();
      auto fail = info[1].ToNumber().Int32Value();
      auto zfail = info[2].ToNumber().Int32Value();
      auto zpass = info[3].ToNumber().Int32Value();
      glContext_->stencilOpSeparate(face, fail, zfail, zpass);
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::BlendColor(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 4)
    {
      float r = info[0].ToNumber().FloatValue();
      float g = info[1].ToNumber().FloatValue();
      float b = info[2].ToNumber().FloatValue();
      float a = info[3].ToNumber().FloatValue();
      glContext_->blendColor(r, g, b, a);
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::BlendEquation(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto mode = info[0].ToNumber().Int32Value();
    glContext_->blendEquation(mode);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::BlendEquationSeparate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto modeRGB = info[0].ToNumber().Int32Value();
    auto modeAlpha = info[1].ToNumber().Int32Value();
    glContext_->blendEquationSeparate(modeRGB, modeAlpha);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::BlendFunc(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto sfactor = info[0].ToNumber().Int32Value();
    auto dfactor = info[1].ToNumber().Int32Value();
    glContext_->blendFunc(sfactor, dfactor);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::BlendFuncSeparate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto srcRGB = info[0].ToNumber().Int32Value();
    auto dstRGB = info[1].ToNumber().Int32Value();
    auto srcAlpha = info[2].ToNumber().Int32Value();
    auto dstAlpha = info[3].ToNumber().Int32Value();
    glContext_->blendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::ColorMask(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto r = info[0].ToBoolean().Value();
    auto g = info[1].ToBoolean().Value();
    auto b = info[2].ToBoolean().Value();
    auto a = info[3].ToBoolean().Value();
    glContext_->colorMask(r, g, b, a);
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::CullFace(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1 && info[0].IsNumber())
      glContext_->cullFace(info[0].ToNumber().Int32Value());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::FrontFace(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1 && info[0].IsNumber())
      glContext_->frontFace(info[0].ToNumber().Int32Value());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::Enable(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1 && info[0].IsNumber())
      glContext_->enable(info[0].ToNumber().Int32Value());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::Disable(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1 && info[0].IsNumber())
      glContext_->disable(info[0].ToNumber().Int32Value());
    return env.Undefined();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::GetParameter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 && info[0].IsNumber())
    {
      Napi::TypeError::New(env, "getParameter() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Value value = env.Undefined();
    uint32_t pname = info[0].ToNumber().Uint32Value();
    switch (pname)
    {
    /**
     * GLint
     */
    case WEBGL_ALPHA_BITS:
    case WEBGL_BLUE_BITS:
    case WEBGL_RED_BITS:
    case WEBGL_DEPTH_BITS:
    case WEBGL_GREEN_BITS:
    case WEBGL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
    case WEBGL_MAX_CUBE_MAP_TEXTURE_SIZE:
    case WEBGL_MAX_FRAGMENT_UNIFORM_VECTORS:
    case WEBGL_MAX_RENDERBUFFER_SIZE:
    case WEBGL_MAX_TEXTURE_IMAGE_UNITS:
    case WEBGL_MAX_TEXTURE_SIZE:
    case WEBGL_MAX_VARYING_VECTORS:
    case WEBGL_MAX_VERTEX_ATTRIBS:
    case WEBGL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:
    case WEBGL_MAX_VERTEX_UNIFORM_VECTORS:
    case WEBGL_PACK_ALIGNMENT:
    case WEBGL_SAMPLE_BUFFERS:
    case WEBGL_SAMPLES:
    case WEBGL_STENCIL_BACK_REF:
    case WEBGL_STENCIL_BITS:
    case WEBGL_STENCIL_CLEAR_VALUE:
    case WEBGL_STENCIL_REF:
    case WEBGL_SUBPIXEL_BITS:
    case WEBGL_UNPACK_ALIGNMENT:
    /**
     * GLenum
     */
    case WEBGL_ACTIVE_TEXTURE:
    case WEBGL_BLEND_DST_ALPHA:
    case WEBGL_BLEND_DST_RGB:
    case WEBGL_BLEND_EQUATION:
    // case WEBGL_BLEND_EQUATION_RGB: /** same as BLEND_EQUATION */
    case WEBGL_BLEND_EQUATION_ALPHA:
    case WEBGL_BLEND_SRC_ALPHA:
    case WEBGL_BLEND_SRC_RGB:
    case WEBGL_CULL_FACE_MODE:
    case WEBGL_DEPTH_FUNC:
    case WEBGL_FRONT_FACE:
    case WEBGL_GENERATE_MIPMAP_HINT:
    case WEBGL_IMPLEMENTATION_COLOR_READ_FORMAT:
    case WEBGL_IMPLEMENTATION_COLOR_READ_TYPE:
    case WEBGL_STENCIL_BACK_FAIL:
    case WEBGL_STENCIL_BACK_FUNC:
    case WEBGL_STENCIL_BACK_PASS_DEPTH_FAIL:
    case WEBGL_STENCIL_BACK_PASS_DEPTH_PASS:
    case WEBGL_STENCIL_FAIL:
    case WEBGL_STENCIL_FUNC:
    case WEBGL_STENCIL_PASS_DEPTH_FAIL:
    case WEBGL_STENCIL_PASS_DEPTH_PASS:
    case WEBGL_UNPACK_COLORSPACE_CONVERSION_WEBGL:
    {
      value = Napi::Number::New(env, glContext_->getParameter(static_cast<client_graphics::WebGLIntegerParameterName>(pname)));
      break;
    }
    /**
     * GLboolean
     */
    case WEBGL_BLEND:
    case WEBGL_CULL_FACE:
    case WEBGL_DEPTH_TEST:
    case WEBGL_DEPTH_WRITEMASK:
    case WEBGL_DITHER:
    case WEBGL_POLYGON_OFFSET_FILL:
    case WEBGL_SAMPLE_COVERAGE_INVERT:
    case WEBGL_SCISSOR_TEST:
    case WEBGL_STENCIL_TEST:
    case WEBGL_UNPACK_FLIP_Y_WEBGL:
    case WEBGL_UNPACK_PREMULTIPLY_ALPHA_WEBGL:
    {
      value = Napi::Boolean::New(env, glContext_->getParameter(static_cast<client_graphics::WebGLBooleanParameterName>(pname)));
      break;
    }
    /**
     * GLfloat[]
     */
    case WEBGL_VIEWPORT:
    case WEBGL_SCISSOR_BOX:
    {
      auto values = glContext_->getParameter(static_cast<client_graphics::WebGLFloatArrayParameterName>(pname));
      auto array = Napi::Float32Array::New(env, values.size());
      for (size_t i = 0; i < values.size(); i++)
        array.Set(i, Napi::Number::New(env, values[i]));
      value = array;
      break;
    }
    /**
     * GLstring
     */
    case WEBGL_RENDERER:
    case WEBGL_SHADING_LANGUAGE_VERSION:
    case WEBGL_VENDOR:
    case WEBGL_VERSION:
    {
      value = Napi::String::New(env, glContext_->getParameter(static_cast<client_graphics::WebGLStringParameterName>(pname)));
      break;
    }
    default:
      std::cerr << "Unknown pname: " << pname << std::endl;
      break;
    }

    return value;
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::GetShaderPrecisionFormat(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "getShaderPrecisionFormat() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int shadertype = info[0].As<Napi::Number>().Int32Value();
    int precisiontype = info[1].As<Napi::Number>().Int32Value();
    return WebGLShaderPrecisionFormat::NewInstance(env,
                                                   glContext_->getShaderPrecisionFormat(shadertype, precisiontype));
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::GetError(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, glContext_->getError());
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::GetSupportedExtensions(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto jsThis = info.This().ToObject();
    if (jsThis.Has("_extensions") && jsThis.Get("_extensions").IsArray())
      return jsThis.Get("_extensions");

    auto extensions = glContext_->getSupportedExtensions();
    auto jsList = Napi::Array::New(env, extensions.size());
    for (size_t i = 0; i < extensions.size(); i++)
      jsList.Set(i, Napi::String::New(env, extensions[i]));

    // Update the extensions array to the context object.
    jsThis.Set("_extensions", jsList);
    return jsList;
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::DrawingBufferWidthGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, glContext_->drawingBufferWidth());
  }

  template <typename ObjectType, typename ContextType>
  void WebGLBaseRenderingContext<ObjectType, ContextType>::DrawingBufferWidthSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::TypeError::New(env, "drawingBufferWidth is readonly.")
      .ThrowAsJavaScriptException();
  }

  template <typename ObjectType, typename ContextType>
  Napi::Value WebGLBaseRenderingContext<ObjectType, ContextType>::DrawingBufferHeightGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, glContext_->drawingBufferHeight());
  }

  template <typename ObjectType, typename ContextType>
  void WebGLBaseRenderingContext<ObjectType, ContextType>::DrawingBufferHeightSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::TypeError::New(env, "drawingBufferHeight is readonly.")
      .ThrowAsJavaScriptException();
  }

  template <typename ObjectType, typename ContextType>
  std::shared_ptr<canvas::ImageSource> WebGLBaseRenderingContext<ObjectType, ContextType>::getImageSourceFromObject(Napi::Env env, Napi::Object imageSourceObject)
  {
    if (imageSourceObject.InstanceOf(canvasbinding::ImageBitmap::constructor->Value()))
      return canvasbinding::ImageBitmap::Unwrap(imageSourceObject)->getImageSource();
    else if (imageSourceObject.InstanceOf(canvasbinding::ImageData::constructor->Value()))
      return canvasbinding::ImageData::Unwrap(imageSourceObject)->getImageSource();
    else if (imageSourceObject.InstanceOf(canvasbinding::OffscreenCanvas::constructor->Value()))
      return canvasbinding::OffscreenCanvas::Unwrap(imageSourceObject)->getImageSource();
    else if (imageSourceObject.InstanceOf(dombinding::HTMLCanvasElement::constructor->Value()))
      return dombinding::HTMLCanvasElement::Unwrap(imageSourceObject)->getImageSource();
    else if (imageSourceObject.InstanceOf(dombinding::HTMLImageElement::constructor->Value()))
      return dombinding::HTMLImageElement::Unwrap(imageSourceObject)->getImageSource();
    else
    {
      // Prints the object if not supported.
      env.Global()
        .Get("console")
        .As<Napi::Object>()
        .Get("log")
        .As<Napi::Function>()({imageSourceObject});

      /**
       * TODO(yorkie): support HTMLVideoElement
       */
      return nullptr;
    }
  }

}
