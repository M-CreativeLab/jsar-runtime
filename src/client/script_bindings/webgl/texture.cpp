#include "./texture.hpp"

namespace endor
{
  namespace script_bindings
  {
    namespace webgl_bindings
    {
      using namespace std;
      using namespace v8;

      void WebGLTexture::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
      {
      }

      Local<Object> WebGLTexture::NewInstance(Isolate *isolate,
                                              shared_ptr<client_graphics::WebGLTexture> nativeTexture)
      {
        EscapableHandleScope scope(isolate);
        return nativeTexture != nullptr
                 ? scope.Escape(WebGLTextureBase::NewInstance(isolate, nativeTexture).As<Object>())
                 : scope.Escape(Local<Object>());
      }

      WebGLTexture::WebGLTexture(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
          : WebGLTextureBase(isolate, args)
      {
      }
    } // namespace webgl
  }   // namespace script_bindings
} // namespace endor
