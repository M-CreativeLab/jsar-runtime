#include "./rendering_context.hpp"
#include "./document.hpp"

namespace dombinding
{
  Napi::FunctionReference *DocumentRenderingContext::constructor;

  inline v8::Local<v8::Value> V8LocalValueFromJsValue(napi_value v)
  {
    v8::Local<v8::Value> local;
    memcpy(static_cast<void *>(&local), &v, sizeof(v));
    return local;
  }

  void DocumentRenderingContext::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function func = DefineClass(env, "DocumentRenderingContext",
                                      {InstanceMethod("start", &DocumentRenderingContext::Start)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("DocumentRenderingContext", func);
  }

  DocumentRenderingContext::DocumentRenderingContext(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<DocumentRenderingContext>(info),
        renderingContext(std::make_shared<dom::DocumentRenderingContext>())
  {
  }

  DocumentRenderingContext::~DocumentRenderingContext()
  {
  }

  Napi::Value DocumentRenderingContext::Start(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 0)
    {
      Napi::TypeError::New(env, "Expected a string at least").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto jsSourceString = info[0].As<Napi::String>();
    dom::DOMParsingType parsingType = dom::DOMParsingType::HTML;
    if (info.Length() >= 2 && info[1].IsString())
    {
      auto jsMimeTypeString = info[1].As<Napi::String>();
      std::string mimeType = jsMimeTypeString.Utf8Value();
      if (mimeType == "text/xml" || mimeType == "application/xml")
        parsingType = dom::DOMParsingType::XML;
      else if (mimeType == "image/svg+xml")
        parsingType = dom::DOMParsingType::SVG;
      else if (mimeType == "text/html")
        parsingType = dom::DOMParsingType::HTML;
      else
      {
        auto msg = "Unknown mime type: " + mimeType;
        Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }

    if (parsingType == dom::DOMParsingType::HTML)
    {
      try
      {
        auto doc = renderingContext->create<dom::HTMLDocument>(jsSourceString.Utf8Value(), parsingType);
        auto jsInstance = Document::NewInstance(env, doc);
        {
          auto scriptingContext = renderingContext->scriptingContext;
          scriptingContext->setDocumentValue(V8LocalValueFromJsValue(jsInstance));
          scriptingContext->makeContext();
        }
        renderingContext->open(doc);
        return jsInstance;
      }
      catch (const std::exception &e)
      {
        Napi::TypeError::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }
    else
    {
      Napi::TypeError::New(env, "Only HTML document is supported by DocumentRenderingContext").ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }
}
