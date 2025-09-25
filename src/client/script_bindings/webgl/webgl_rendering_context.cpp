#include "./webgl_rendering_context.hpp"

namespace script_bindings
{
  namespace webgl_bindings
  {
    using namespace std;
    using namespace v8;

    void WebGLRenderingContext::SetupConstants(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      // WebGL 1.0 constants as per MDN spec
      auto prototype = tpl->PrototypeTemplate();

      // Buffer types
      prototype->Set(String::NewFromUtf8(isolate, "ARRAY_BUFFER").ToLocalChecked(),
                     Integer::New(isolate, 0x8892));
      prototype->Set(String::NewFromUtf8(isolate, "ELEMENT_ARRAY_BUFFER").ToLocalChecked(),
                     Integer::New(isolate, 0x8893));

      // Shader types
      prototype->Set(String::NewFromUtf8(isolate, "VERTEX_SHADER").ToLocalChecked(),
                     Integer::New(isolate, 0x8B31));
      prototype->Set(String::NewFromUtf8(isolate, "FRAGMENT_SHADER").ToLocalChecked(),
                     Integer::New(isolate, 0x8B30));

      // Texture types
      prototype->Set(String::NewFromUtf8(isolate, "TEXTURE_2D").ToLocalChecked(),
                     Integer::New(isolate, 0x0DE1));

      // Data types
      prototype->Set(String::NewFromUtf8(isolate, "FLOAT").ToLocalChecked(),
                     Integer::New(isolate, 0x1406));
      prototype->Set(String::NewFromUtf8(isolate, "UNSIGNED_BYTE").ToLocalChecked(),
                     Integer::New(isolate, 0x1401));

      // Clear bits
      prototype->Set(String::NewFromUtf8(isolate, "COLOR_BUFFER_BIT").ToLocalChecked(),
                     Integer::New(isolate, 0x00004000));
      prototype->Set(String::NewFromUtf8(isolate, "DEPTH_BUFFER_BIT").ToLocalChecked(),
                     Integer::New(isolate, 0x00000100));

      // Drawing modes
      prototype->Set(String::NewFromUtf8(isolate, "TRIANGLES").ToLocalChecked(),
                     Integer::New(isolate, 0x0004));
      prototype->Set(String::NewFromUtf8(isolate, "TRIANGLE_STRIP").ToLocalChecked(),
                     Integer::New(isolate, 0x0005));
    }

    void WebGLRenderingContext::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      // Set up the WebGL 1.0 API methods and properties

      // Buffer operations
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "createBuffer").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, CreateBuffer));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "deleteBuffer").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, DeleteBuffer));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "bindBuffer").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, BindBuffer));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "bufferData").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, BufferData));

      // Shader operations
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "createShader").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, CreateShader));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "deleteShader").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, DeleteShader));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "shaderSource").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, ShaderSource));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "compileShader").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, CompileShader));

      // Program operations
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "createProgram").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, CreateProgram));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "deleteProgram").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, DeleteProgram));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "attachShader").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, AttachShader));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "linkProgram").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, LinkProgram));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "useProgram").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, UseProgram));

      // Texture operations
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "createTexture").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, CreateTexture));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "deleteTexture").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, DeleteTexture));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "bindTexture").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, BindTexture));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "texImage2D").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, TexImage2D));

      // Drawing operations
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "clear").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, Clear));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "clearColor").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, ClearColor));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "viewport").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, Viewport));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "drawArrays").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, DrawArrays));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "drawElements").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, DrawElements));

      // Uniform operations
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "getUniformLocation").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, GetUniformLocation));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "uniform1f").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, Uniform1f));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "uniform1i").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, Uniform1i));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "uniformMatrix4fv").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, UniformMatrix4fv));

      // Error and state operations
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "getError").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, GetError));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "enable").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, Enable));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "disable").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, Disable));

      // Canvas property
      tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "canvas").ToLocalChecked(),
                                           CanvasGetter);

      // WebGL constants
      SetupConstants(isolate, tpl);
    }

    Local<Object> WebGLRenderingContext::NewInstance(Isolate *isolate,
                                                     shared_ptr<client_graphics::WebGLContext> nativeContext)
    {
      EscapableHandleScope scope(isolate);
      if (nativeContext == nullptr)
      {
        return scope.Escape(Local<Object>());
      }
      else
      {
        return scope.Escape(WebGLRenderingContextBase::NewInstance(isolate, nativeContext).As<Object>());
      }
    }

    WebGLRenderingContext::WebGLRenderingContext(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : WebGLRenderingContextBase(isolate, args)
    {
    }

    // Method implementations - stubs for now, would need native implementation
    void WebGLRenderingContext::CreateBuffer(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      // TODO: Implement native WebGL buffer creation
      args.GetReturnValue().SetNull();
    }

    void WebGLRenderingContext::CreateShader(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      // TODO: Implement native WebGL shader creation
      args.GetReturnValue().SetNull();
    }

    void WebGLRenderingContext::CreateProgram(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      // TODO: Implement native WebGL program creation
      args.GetReturnValue().SetNull();
    }

    void WebGLRenderingContext::CreateTexture(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      // TODO: Implement native WebGL texture creation
      args.GetReturnValue().SetNull();
    }

    void WebGLRenderingContext::Clear(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      // TODO: Implement native WebGL clear
    }

    void WebGLRenderingContext::Viewport(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      // TODO: Implement native WebGL viewport
    }

    void WebGLRenderingContext::DrawArrays(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      // TODO: Implement native WebGL draw arrays
    }

    void WebGLRenderingContext::CanvasGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      // TODO: Return associated canvas object
      info.GetReturnValue().SetNull();
    }

    // Stub implementations for other methods
    void WebGLRenderingContext::DeleteBuffer(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::BindBuffer(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::BufferData(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::DeleteShader(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::ShaderSource(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::CompileShader(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::DeleteProgram(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::AttachShader(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::LinkProgram(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::UseProgram(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::DeleteTexture(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::BindTexture(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::TexImage2D(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::ClearColor(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::DrawElements(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::GetUniformLocation(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::Uniform1f(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::Uniform1i(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::UniformMatrix4fv(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::GetError(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::Enable(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::Disable(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
  }
}