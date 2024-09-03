#include "./document.hpp"

namespace dombinding
{
  Napi::FunctionReference *Document::constructor;
  void Document::Init(Napi::Env env, Napi::Object exports)
  {
    BaseDocument<Document, dom::HTMLDocument>::Init(env, "Document");
  }

  Document::Document(const Napi::CallbackInfo &info) : BaseDocument<Document, dom::HTMLDocument>(info)
  {
  }

  Napi::FunctionReference *XMLDocument::constructor;
  void XMLDocument::Init(Napi::Env env, Napi::Object exports)
  {
    BaseDocument<XMLDocument, dom::XMLDocument>::Init(env, "XMLDocument");
  }

  XMLDocument::XMLDocument(const Napi::CallbackInfo &info) : BaseDocument<XMLDocument, dom::XMLDocument>(info)
  {
  }
}
