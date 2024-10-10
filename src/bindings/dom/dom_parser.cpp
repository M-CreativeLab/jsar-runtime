#include "./dom_parser.hpp"
#include "./document.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *DOMParser::constructor;
  void DOMParser::Init(Napi::Env env)
  {
    Napi::Function func = DefineClass(env, "DOMParser",
                                      {InstanceMethod("parseFromString", &DOMParser::ParseFromString)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("DOMParser", func);
  }

  DOMParser::DOMParser(const Napi::CallbackInfo &info) : Napi::ObjectWrap<DOMParser>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
  }

  DOMParser::~DOMParser()
  {
  }

  Napi::Value DOMParser::ParseFromString(const Napi::CallbackInfo &info)
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
      auto htmlDoc = parser.parseFromString<dom::HTMLDocument>(jsSourceString.Utf8Value(), parsingType);
      return Document::NewInstance(env, htmlDoc);
    }
    else if (parsingType == dom::DOMParsingType::XML)
    {
      auto xmlDoc = parser.parseFromString<dom::XMLDocument>(jsSourceString.Utf8Value(), parsingType);
      return XMLDocument::NewInstance(env, xmlDoc);
    }
    else
    {
      Napi::TypeError::New(env, "SVG parsing is not supported yet").ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }
}
