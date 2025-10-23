#pragma once

#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/fetch/response.hpp>

namespace script_bindings
{
  class Response;
  using ResponseBase = scripting_base::ObjectWrap<Response, client_fetch::Response>;

  class Response : public ResponseBase
  {
    using ResponseBase::ObjectWrap;

  public:
    static std::string Name()
    {
      return "Response";
    }
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
    /**
     * Creates a new Response instance from a source response JavaScript object.
     */
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, v8::Local<v8::Value> source_response);
    /**
     * Creates a new Response instance from a Blob object.
     */
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_fileapi::Blob> blob);

  private:
    void SourcePropertyGetter(const char *name, const v8::FunctionCallbackInfo<v8::Value> &args);
    void SourceMethodCall(const char *name,
                          const v8::FunctionCallbackInfo<v8::Value> &args,
                          v8::Local<v8::Promise::Resolver> resolver);

    // Properties
    void BodyGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
    void BodyUsedGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
    void HeadersGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
    void OkGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
    void RedirectedGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
    void StatusGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
    void StatusTextGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
    void TypeGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
    void UrlGetter(const v8::FunctionCallbackInfo<v8::Value> &info);

    // Methods
    void ArrayBuffer(const v8::FunctionCallbackInfo<v8::Value> &args);
    void Blob(const v8::FunctionCallbackInfo<v8::Value> &args);
    void Bytes(const v8::FunctionCallbackInfo<v8::Value> &args);
    void Clone(const v8::FunctionCallbackInfo<v8::Value> &args);
    void FormData(const v8::FunctionCallbackInfo<v8::Value> &args);
    void Json(const v8::FunctionCallbackInfo<v8::Value> &args);
    void Text(const v8::FunctionCallbackInfo<v8::Value> &args);

  private:
    v8::Global<v8::Object> source_response_handle_;
  };
}
