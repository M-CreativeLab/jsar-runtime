#include <client/scripting_base/v8_utils.hpp>
#include "./browsing_context.hpp"

using namespace std;

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

  BrowsingContext::BrowsingContext(const Napi::CallbackInfo &info)
      : RuntimeContextBase<BrowsingContext, dom::BrowsingContext>(info)
      , client_context_(TrClientContextPerProcess::Get())
  {
    assert(this->contextImpl != nullptr && "contextImpl should not be null");
    client_context_->browsingContext = this->contextImpl;
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
      string mimeType = jsMimeTypeString.Utf8Value();
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
    string baseURI = "";

    if (info.Length() >= 3 && info[2].IsString())
    {
      string value = info[2].ToString().Utf8Value();
      if (value == "source")
      {
        inputType = dom::InputType::Source;
        if (info.Length() >= 4 && info[3].IsString())
          baseURI = info[3].ToString().Utf8Value();
      }
    }

    if (parsingType == dom::DOMParsingType::HTML)
    {
      try
      {
        auto doc = contextImpl->create<dom::HTMLDocument>(source.Utf8Value(), parsingType, inputType, baseURI);
        contextImpl->setBaseURI(doc->baseURI);

        // Make sure the scripting context is created.
        auto scriptingContext = contextImpl->scriptingContext;
        scriptingContext->makeMainContext(doc, client_context_->window);
        contextImpl->open(doc);

        auto documentValue = scriptingContext->getGlobalProperty(v8::Isolate::GetCurrent(), "document");
        if (documentValue.IsEmpty() || !documentValue->IsObject())
        {
          return env.Undefined();
        }
        else
        {
          return Napi::Value(env, scripting_base::ToNapiValue(documentValue));
        }
      }
      catch (const exception &e)
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
