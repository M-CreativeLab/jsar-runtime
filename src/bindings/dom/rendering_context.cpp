#include "./rendering_context.hpp"
#include "./document.hpp"

namespace dombinding
{
  Napi::FunctionReference *DocumentRenderingContext::constructor;

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
      auto htmlDoc = renderingContext->start<dom::HTMLDocument>(jsSourceString.Utf8Value(), parsingType);
      return Document::NewInstance(env, htmlDoc);
    }
    else
    {
      Napi::TypeError::New(env, "Only HTML document is supported by DocumentRenderingContext").ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }
}
