#include "./browsing_context.hpp"
#include "./document-inl.hpp"
#include "../browser/window.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *BrowsingContext::constructor;
  void BrowsingContext::Init(Napi::Env env, Napi::Object exports)
  {
    auto props = GetClassProperties(env);
    {
      auto newProps = vector<Napi::ClassPropertyDescriptor<BrowsingContext>>({
        InstanceMethod("start", &BrowsingContext::Start),
      });
      props.insert(props.end(), newProps.begin(), newProps.end());
    }
    Napi::Function func = DefineClass(env, "BrowsingContext", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("BrowsingContext", func);
  }

  Napi::Value BrowsingContext::Start(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 0)
    {
      Napi::TypeError::New(env, "Expected a string at least").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto source = info[0].As<Napi::String>();
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

    auto inputType = dom::InputType::URL;
    if (info.Length() >= 3 && info[3].IsString())
    {
      std::string value = info[3].ToString().Utf8Value();
      if (value == "source")
      {
        inputType = dom::InputType::Source;
      }
    }
    if (parsingType == dom::DOMParsingType::HTML)
    {
      try
      {
        auto doc = contextImpl->create<dom::HTMLDocument>(source.Utf8Value(), parsingType, inputType);
        contextImpl->setBaseURI(doc->baseURI);

        auto jsInstance = Document::NewInstance(env, doc);
        {
          auto window = browserbinding::Window::NewInstance(env, doc->baseURI);
          auto scriptingContext = contextImpl->scriptingContext;
          scriptingContext->makeMainContext(convertNapiValueToV8Local(window),
                                            convertNapiValueToV8Local(jsInstance));
        }
        contextImpl->open(doc);
        env.Global().Set("document", jsInstance);
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
