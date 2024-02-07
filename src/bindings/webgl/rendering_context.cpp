#include "rendering_context.hpp"
#include "renderer/command_buffer.hpp"
#include "renderer/render_api.hpp"

namespace webgl
{
  Napi::FunctionReference *WebGLRenderingContext::constructor;

  Napi::Object WebGLRenderingContext::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(
        env,
        "WebGLRenderingContext",
        {/**
          * Constants
          */
         // Draw modes
         InstanceValue("POINTS", Napi::Number::New(env, WEBGL_POINTS)),
         InstanceValue("LINES", Napi::Number::New(env, WEBGL_LINES)),
         InstanceValue("LINE_LOOP", Napi::Number::New(env, WEBGL_LINE_LOOP)),
         InstanceValue("LINE_STRIP", Napi::Number::New(env, WEBGL_LINE_STRIP)),
         InstanceValue("TRIANGLES", Napi::Number::New(env, WEBGL_TRIANGLES)),
         InstanceValue("TRIANGLE_STRIP", Napi::Number::New(env, WEBGL_TRIANGLE_STRIP)),
         InstanceValue("TRIANGLE_FAN", Napi::Number::New(env, WEBGL_TRIANGLE_FAN)),
         // Clear mask
         InstanceValue("COLOR_BUFFER_BIT", Napi::Number::New(env, COLOR_BUFFER_BIT)),
         InstanceValue("DEPTH_BUFFER_BIT", Napi::Number::New(env, DEPTH_BUFFER_BIT)),
         InstanceValue("STENCIL_BUFFER_BIT", Napi::Number::New(env, STENCIL_BUFFER_BIT)),
         // Caps
         InstanceValue("BLEND", Napi::Number::New(env, WEBGL_BLEND)),
         InstanceValue("CULL_FACE", Napi::Number::New(env, WEBGL_CULL_FACE)),
         InstanceValue("DEPTH_TEST", Napi::Number::New(env, WEBGL_DEPTH_TEST)),
         InstanceValue("DITHER", Napi::Number::New(env, WEBGL_DITHER)),
         InstanceValue("POLYGON_OFFSET_FILL", Napi::Number::New(env, WEBGL_POLYGON_OFFSET_FILL)),
         InstanceValue("SAMPLE_ALPHA_TO_COVERAGE", Napi::Number::New(env, WEBGL_SAMPLE_ALPHA_TO_COVERAGE)),
         InstanceValue("SAMPLE_COVERAGE", Napi::Number::New(env, WEBGL_SAMPLE_COVERAGE)),
         InstanceValue("SCISSOR_TEST", Napi::Number::New(env, WEBGL_SCISSOR_TEST)),
         InstanceValue("STENCIL_TEST", Napi::Number::New(env, WEBGL_STENCIL_TEST)),
         // Shader types
         InstanceValue("VERTEX_SHADER", Napi::Number::New(env, WEBGL_VERTEX_SHADER)),
         InstanceValue("FRAGMENT_SHADER", Napi::Number::New(env, WEBGL_FRAGMENT_SHADER)),
         // Buffer types
         InstanceValue("ARRAY_BUFFER", Napi::Number::New(env, WEBGL_ARRAY_BUFFER)),
         InstanceValue("ELEMENT_ARRAY_BUFFER", Napi::Number::New(env, WEBGL_ELEMENT_ARRAY_BUFFER)),
         // Buffer usages
         InstanceValue("STREAM_DRAW", Napi::Number::New(env, WEBGL_STREAM_DRAW)),
         InstanceValue("STATIC_DRAW", Napi::Number::New(env, WEBGL_STATIC_DRAW)),
         InstanceValue("DYNAMIC_DRAW", Napi::Number::New(env, WEBGL_DYNAMIC_DRAW)),
         // Texture types
         InstanceValue("TEXTURE_2D", Napi::Number::New(env, WEBGL_TEXTURE_2D)),
         InstanceValue("TEXTURE_CUBE_MAP_POSITION_X", Napi::Number::New(env, WEBGL_TEXTURE_CUBE_MAP_POSITION_X)),
         InstanceValue("TEXTURE_CUBE_MAP_NEGATIVE_X", Napi::Number::New(env, WEBGL_TEXTURE_CUBE_MAP_NEGATIVE_X)),
         InstanceValue("TEXTURE_CUBE_MAP_POSITION_Y", Napi::Number::New(env, WEBGL_TEXTURE_CUBE_MAP_POSITION_Y)),
         InstanceValue("TEXTURE_CUBE_MAP_NEGATIVE_Y", Napi::Number::New(env, WEBGL_TEXTURE_CUBE_MAP_NEGATIVE_Y)),
         InstanceValue("TEXTURE_CUBE_MAP_POSITION_Z", Napi::Number::New(env, WEBGL_TEXTURE_CUBE_MAP_POSITION_Z)),
         InstanceValue("TEXTURE_CUBE_MAP_NEGATIVE_Z", Napi::Number::New(env, WEBGL_TEXTURE_CUBE_MAP_NEGATIVE_Z)),
         // Texture ids
         InstanceValue("TEXTURE0", Napi::Number::New(env, WEBGL_TEXTURE0)),
         InstanceValue("TEXTURE1", Napi::Number::New(env, WEBGL_TEXTURE1)),
         InstanceValue("TEXTURE2", Napi::Number::New(env, WEBGL_TEXTURE2)),
         InstanceValue("TEXTURE3", Napi::Number::New(env, WEBGL_TEXTURE3)),
         InstanceValue("TEXTURE4", Napi::Number::New(env, WEBGL_TEXTURE4)),
         InstanceValue("TEXTURE5", Napi::Number::New(env, WEBGL_TEXTURE5)),
         InstanceValue("TEXTURE6", Napi::Number::New(env, WEBGL_TEXTURE6)),
         InstanceValue("TEXTURE7", Napi::Number::New(env, WEBGL_TEXTURE7)),
         InstanceValue("TEXTURE8", Napi::Number::New(env, WEBGL_TEXTURE8)),
         InstanceValue("TEXTURE9", Napi::Number::New(env, WEBGL_TEXTURE9)),
         InstanceValue("TEXTURE10", Napi::Number::New(env, WEBGL_TEXTURE10)),
         InstanceValue("TEXTURE11", Napi::Number::New(env, WEBGL_TEXTURE11)),
         InstanceValue("TEXTURE12", Napi::Number::New(env, WEBGL_TEXTURE12)),
         InstanceValue("TEXTURE13", Napi::Number::New(env, WEBGL_TEXTURE13)),
         InstanceValue("TEXTURE14", Napi::Number::New(env, WEBGL_TEXTURE14)),
         InstanceValue("TEXTURE15", Napi::Number::New(env, WEBGL_TEXTURE15)),
         InstanceValue("TEXTURE16", Napi::Number::New(env, WEBGL_TEXTURE16)),
         InstanceValue("TEXTURE17", Napi::Number::New(env, WEBGL_TEXTURE17)),
         InstanceValue("TEXTURE18", Napi::Number::New(env, WEBGL_TEXTURE18)),
         InstanceValue("TEXTURE19", Napi::Number::New(env, WEBGL_TEXTURE19)),
         InstanceValue("TEXTURE20", Napi::Number::New(env, WEBGL_TEXTURE20)),
         InstanceValue("TEXTURE21", Napi::Number::New(env, WEBGL_TEXTURE21)),
         InstanceValue("TEXTURE22", Napi::Number::New(env, WEBGL_TEXTURE22)),
         InstanceValue("TEXTURE23", Napi::Number::New(env, WEBGL_TEXTURE23)),
         InstanceValue("TEXTURE24", Napi::Number::New(env, WEBGL_TEXTURE24)),
         InstanceValue("TEXTURE25", Napi::Number::New(env, WEBGL_TEXTURE25)),
         InstanceValue("TEXTURE26", Napi::Number::New(env, WEBGL_TEXTURE26)),
         InstanceValue("TEXTURE27", Napi::Number::New(env, WEBGL_TEXTURE27)),
         InstanceValue("TEXTURE28", Napi::Number::New(env, WEBGL_TEXTURE28)),
         InstanceValue("TEXTURE29", Napi::Number::New(env, WEBGL_TEXTURE29)),
         InstanceValue("TEXTURE30", Napi::Number::New(env, WEBGL_TEXTURE30)),
         InstanceValue("TEXTURE31", Napi::Number::New(env, WEBGL_TEXTURE31)),
         // Texture formats
         InstanceValue("RGBA", Napi::Number::New(env, WEBGL_RGBA)),
         InstanceValue("RGB", Napi::Number::New(env, WEBGL_RGB)),
         InstanceValue("ALPHA", Napi::Number::New(env, WEBGL_ALPHA)),
         InstanceValue("LUMINANCE", Napi::Number::New(env, WEBGL_LUMINANCE)),
         InstanceValue("LUMINANCE_ALPHA", Napi::Number::New(env, WEBGL_LUMINANCE_ALPHA)),
         // Texture data types
         InstanceValue("UNSIGNED_BYTE", Napi::Number::New(env, WEBGL_UNSIGNED_BYTE)),
         InstanceValue("UNSIGNED_SHORT_5_6_5", Napi::Number::New(env, WEBGL_UNSIGNED_SHORT_5_6_5)),
         InstanceValue("UNSIGNED_SHORT_4_4_4_4", Napi::Number::New(env, WEBGL_UNSIGNED_SHORT_4_4_4_4)),
         InstanceValue("UNSIGNED_SHORT_5_5_5_1", Napi::Number::New(env, WEBGL_UNSIGNED_SHORT_5_5_5_1)),
         // Texture parameter names
         InstanceValue("TEXTURE_MAG_FILTER", Napi::Number::New(env, WEBGL_TEXTURE_MAG_FILTER)),
         InstanceValue("TEXTURE_MIN_FILTER", Napi::Number::New(env, WEBGL_TEXTURE_MIN_FILTER)),
         InstanceValue("TEXTURE_WRAP_S", Napi::Number::New(env, WEBGL_TEXTURE_WRAP_S)),
         InstanceValue("TEXTURE_WRAP_T", Napi::Number::New(env, WEBGL_TEXTURE_WRAP_T)),
         // Texture parameter values
         InstanceValue("NEAREST", Napi::Number::New(env, WEBGL_NEAREST)),
         InstanceValue("LINEAR", Napi::Number::New(env, WEBGL_LINEAR)),
         InstanceValue("NEAREST_MIPMAP_NEAREST", Napi::Number::New(env, WEBGL_NEAREST_MIPMAP_NEAREST)),
         InstanceValue("LINEAR_MIPMAP_NEAREST", Napi::Number::New(env, WEBGL_LINEAR_MIPMAP_NEAREST)),
         InstanceValue("NEAREST_MIPMAP_LINEAR", Napi::Number::New(env, WEBGL_NEAREST_MIPMAP_LINEAR)),
         InstanceValue("LINEAR_MIPMAP_LINEAR", Napi::Number::New(env, WEBGL_LINEAR_MIPMAP_LINEAR)),
         InstanceValue("REPEAT", Napi::Number::New(env, WEBGL_REPEAT)),
         InstanceValue("CLAMP_TO_EDGE", Napi::Number::New(env, WEBGL_CLAMP_TO_EDGE)),
         InstanceValue("MIRRORED_REPEAT", Napi::Number::New(env, WEBGL_MIRRORED_REPEAT)),
         // Vertex attribute types
         InstanceValue("BYTE", Napi::Number::New(env, WEBGL_BYTE)),
         InstanceValue("UNSIGNED_BYTE", Napi::Number::New(env, WEBGL_UNSIGNED_BYTE)),
         InstanceValue("SHORT", Napi::Number::New(env, WEBGL_SHORT)),
         InstanceValue("UNSIGNED_SHORT", Napi::Number::New(env, WEBGL_UNSIGNED_SHORT)),
         InstanceValue("FLOAT", Napi::Number::New(env, WEBGL_FLOAT)),
         // Depth test functions
         InstanceValue("NEVER", Napi::Number::New(env, WEBGL_NEVER)),
         InstanceValue("LESS", Napi::Number::New(env, WEBGL_LESS)),
         InstanceValue("EQUAL", Napi::Number::New(env, WEBGL_EQUAL)),
         InstanceValue("LEQUAL", Napi::Number::New(env, WEBGL_LEQUAL)),
         InstanceValue("GREATER", Napi::Number::New(env, WEBGL_GREATER)),
         InstanceValue("NOTEQUAL", Napi::Number::New(env, WEBGL_NOTEQUAL)),
         InstanceValue("GEQUAL", Napi::Number::New(env, WEBGL_GEQUAL)),
         InstanceValue("ALWAYS", Napi::Number::New(env, WEBGL_ALWAYS)),
         /**
          * Methods
          */
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
         InstanceMethod("bufferData", &WebGLRenderingContext::BufferData),
         InstanceMethod("createTexture", &WebGLRenderingContext::CreateTexture),
         InstanceMethod("bindTexture", &WebGLRenderingContext::BindTexture),
         InstanceMethod("texImage2D", &WebGLRenderingContext::TexImage2D),
         InstanceMethod("texParameteri", &WebGLRenderingContext::TexParameteri),
         InstanceMethod("activeTexture", &WebGLRenderingContext::ActiveTexture),
         InstanceMethod("generateMipmap", &WebGLRenderingContext::GenerateMipmap),
         InstanceMethod("enableVertexAttribArray", &WebGLRenderingContext::EnableVertexAttribArray),
         InstanceMethod("vertexAttribPointer", &WebGLRenderingContext::VertexAttribPointer),
         InstanceMethod("getAttribLocation", &WebGLRenderingContext::GetAttribLocation),
         InstanceMethod("getUniformLocation", &WebGLRenderingContext::GetUniformLocation),
         InstanceMethod("uniformMatrix4fv", &WebGLRenderingContext::UniformMatrix4fv),
         InstanceMethod("drawArrays", &WebGLRenderingContext::DrawArrays),
         InstanceMethod("drawElements", &WebGLRenderingContext::DrawElements),
         InstanceMethod("viewport", &WebGLRenderingContext::Viewport),
         InstanceMethod("clearColor", &WebGLRenderingContext::ClearColor),
         InstanceMethod("clearDepth", &WebGLRenderingContext::ClearDepth),
         InstanceMethod("clearStencil", &WebGLRenderingContext::ClearStencil),
         InstanceMethod("clear", &WebGLRenderingContext::Clear),
         InstanceMethod("depthFunc", &WebGLRenderingContext::DepthFunc),
         InstanceMethod("enable", &WebGLRenderingContext::Enable),
         InstanceMethod("disable", &WebGLRenderingContext::Disable),
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

    auto commandBuffer = new renderer::CreateProgramCommandBuffer();
    m_renderAPI->AddCommandBuffer(commandBuffer);
    commandBuffer->WaitFinished();
    return Napi::Number::New(env, commandBuffer->m_ProgramId);
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
    m_renderAPI->AddCommandBuffer(new renderer::LinkProgramCommandBuffer(program));
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
    m_renderAPI->AddCommandBuffer(new renderer::UseProgramCommandBuffer(program));
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
    m_renderAPI->AddCommandBuffer(new renderer::AttachShaderCommandBuffer(program, shader));
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
    m_renderAPI->AddCommandBuffer(new renderer::DetachShaderCommandBuffer(program, shader));
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

    auto commandBuffer = new renderer::CreateShaderCommandBuffer(type);
    m_renderAPI->AddCommandBuffer(commandBuffer);
    commandBuffer->WaitFinished();
    DEBUG("transmute", "shader id: %d", commandBuffer->m_ShaderId);
    return Napi::Number::New(env, commandBuffer->m_ShaderId);
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
    auto commandBuffer = new renderer::DeleteShaderCommandBuffer(shader);
    m_renderAPI->AddCommandBuffer(commandBuffer);
    commandBuffer->WaitFinished();
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

    auto commandBuffer = new renderer::ShaderSourceCommandBuffer(shader, source.c_str(), source.length());
    m_renderAPI->AddCommandBuffer(commandBuffer);
    DEBUG("transmute", "shader source: %s", source.c_str());
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
    m_renderAPI->AddCommandBuffer(new renderer::CompileShaderCommandBuffer(shader));
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::CreateBuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto commandBuffer = new renderer::CreateBufferCommandBuffer();
    m_renderAPI->AddCommandBuffer(commandBuffer);
    commandBuffer->WaitFinished();
    return Napi::Number::New(env, commandBuffer->m_BufferId);
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
    m_renderAPI->AddCommandBuffer(new renderer::BindBufferCommandBuffer(target, buffer));
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::BufferData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "bufferData() takes 3 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int target = info[0].As<Napi::Number>().Int32Value();
    Napi::ArrayBuffer buffer = info[1].As<Napi::ArrayBuffer>();
    int usage = info[2].As<Napi::Number>().Int32Value();

    auto commandBuffer = new renderer::BufferDataCommandBuffer(
        target,
        buffer.ByteLength(),
        buffer.Data(),
        usage);
    m_renderAPI->AddCommandBuffer(commandBuffer);
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::CreateTexture(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto commandBuffer = new renderer::CreateTextureCommandBuffer();
    m_renderAPI->AddCommandBuffer(commandBuffer);
    commandBuffer->WaitFinished();
    return Napi::Number::New(env, commandBuffer->m_TextureId);
  }

  Napi::Value WebGLRenderingContext::BindTexture(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "bindTexture() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int target = info[0].As<Napi::Number>().Int32Value();
    int texture = info[1].As<Napi::Number>().Int32Value();
    m_renderAPI->AddCommandBuffer(new renderer::BindTextureCommandBuffer(target, texture));
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::TexImage2D(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int target = info[0].As<Napi::Number>().Int32Value();
    int level = info[1].As<Napi::Number>().Int32Value();
    int internalformat = info[2].As<Napi::Number>().Int32Value();
    int width = info[3].As<Napi::Number>().Int32Value();
    int height = info[4].As<Napi::Number>().Int32Value();
    int border = info[5].As<Napi::Number>().Int32Value();
    int format = info[6].As<Napi::Number>().Int32Value();
    int type = info[7].As<Napi::Number>().Int32Value();

    if (!info[8].IsTypedArray())
    {
      Napi::TypeError::New(env, "the pixels should be a TypedArray.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Napi::TypedArray pixels = info[8].As<Napi::TypedArray>();
    Napi::ArrayBuffer data = pixels.ArrayBuffer();
    auto commandBuffer = new renderer::TexImage2DCommandBuffer(
        target,
        level,
        internalformat,
        width,
        height,
        border,
        format,
        type,
        data.ByteLength(),
        data.Data());
    m_renderAPI->AddCommandBuffer(commandBuffer);
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::TexParameteri(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int target = info[0].As<Napi::Number>().Int32Value();
    int pname = info[1].As<Napi::Number>().Int32Value();
    int param = info[2].As<Napi::Number>().Int32Value();
    m_renderAPI->AddCommandBuffer(new renderer::TexParameteriCommandBuffer(target, pname, param));
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::ActiveTexture(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int texture = info[0].As<Napi::Number>().Int32Value();
    m_renderAPI->AddCommandBuffer(new renderer::ActiveTextureCommandBuffer(texture));
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::GenerateMipmap(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int target = info[0].As<Napi::Number>().Int32Value();
    m_renderAPI->AddCommandBuffer(new renderer::GenerateMipmapCommandBuffer(target));
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
    m_renderAPI->AddCommandBuffer(new renderer::EnableVertexAttribArrayCommandBuffer(index));
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::VertexAttribPointer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 6)
    {
      Napi::TypeError::New(env, "vertexAttribPointer() takes 6 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int index = info[0].As<Napi::Number>().Int32Value();
    int size = info[1].As<Napi::Number>().Int32Value();
    int type = info[2].As<Napi::Number>().Int32Value();
    bool normalized = info[3].As<Napi::Boolean>().Value();
    int stride = info[4].As<Napi::Number>().Int32Value();
    int offset = info[5].As<Napi::Number>().Int32Value();

    auto commandBuffer = new renderer::VertexAttribPointerCommandBuffer(
        index,
        size,
        type,
        normalized,
        stride,
        (char *)NULL + offset);
    m_renderAPI->AddCommandBuffer(commandBuffer);
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::GetAttribLocation(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "getAttribLocation() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int program = info[0].As<Napi::Number>().Int32Value();
    std::string name = info[1].As<Napi::String>().Utf8Value();

    auto commandBuffer = new renderer::GetAttribLocationCommandBuffer(program, name.c_str());
    m_renderAPI->AddCommandBuffer(commandBuffer);
    commandBuffer->WaitFinished();
    return Napi::Number::New(env, commandBuffer->m_Location);
  }

  Napi::Value WebGLRenderingContext::GetUniformLocation(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "getUniformLocation() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int program = info[0].As<Napi::Number>().Int32Value();
    std::string name = info[1].As<Napi::String>().Utf8Value();

    auto commandBuffer = new renderer::GetUniformLocationCommandBuffer(program, name.c_str());
    m_renderAPI->AddCommandBuffer(commandBuffer);
    commandBuffer->WaitFinished();
    return Napi::Number::New(env, commandBuffer->m_Location);
  }

  Napi::Value WebGLRenderingContext::UniformMatrix4fv(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "uniformMatrix4fv() takes 3 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int location = info[0].As<Napi::Number>().Int32Value();
    bool transpose = info[1].As<Napi::Boolean>().Value();
    Napi::Float32Array array = info[2].As<Napi::Float32Array>();
    size_t length = array.ElementLength();
    if (length != 16)
    {
      Napi::TypeError::New(env, "uniformMatrix4fv() takes 16 float elements array.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::vector<float> data(length);
    for (size_t i = 0; i < length; i++)
      data[i] = array.Get(i).ToNumber().FloatValue();

    auto commandBuffer = new renderer::UniformMatrix4fvCommandBuffer(
        location,
        transpose,
        data);
    m_renderAPI->AddCommandBuffer(commandBuffer);
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

    m_renderAPI->AddCommandBuffer(new renderer::DrawArraysCommandBuffer(mode, first, count));
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::DrawElements(const Napi::CallbackInfo &info)
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
    m_renderAPI->AddCommandBuffer(new renderer::DrawElementsCommandBuffer(mode, count, type, (char *)NULL + offset));
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
    m_renderAPI->AddCommandBuffer(new renderer::SetViewportCommandBuffer(x, y, width, height));
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
      m_renderAPI->AddCommandBuffer(new renderer::ClearColorCommandBuffer(r, g, b, a));
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
      m_renderAPI->AddCommandBuffer(new renderer::ClearDepthCommandBuffer(depth));
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
      m_renderAPI->AddCommandBuffer(new renderer::ClearStencilCommandBuffer(stencil));
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

  Napi::Value WebGLRenderingContext::DepthFunc(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1)
    {
      uint32_t func = info[0].ToNumber().Uint32Value();
      m_renderAPI->AddCommandBuffer(new renderer::DepthFuncCommandBuffer(func));
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
      m_renderAPI->AddCommandBuffer(new renderer::EnableCommandBuffer(mask));
    }
    return env.Undefined();
  }

  Napi::Value WebGLRenderingContext::Disable(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1)
    {
      uint32_t mask = info[0].ToNumber().Uint32Value();
      m_renderAPI->AddCommandBuffer(new renderer::DisableCommandBuffer(mask));
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
    m_renderAPI->AddCommandBuffer(new renderer::SetScissorCommandBuffer(x, y, width, height));
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
