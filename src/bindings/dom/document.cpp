#include "./document.hpp"

namespace dombinding
{
  Napi::FunctionReference *Document::constructor;
  void Document::Init(Napi::Env env, Napi::Object exports)
  {
    DocumentBase<Document, dom::HTMLDocument>::Init(env, "Document");
  }

  Document::Document(const Napi::CallbackInfo &info) : DocumentBase<Document, dom::HTMLDocument>(info)
  {
  }

  Napi::FunctionReference *XMLDocument::constructor;
  void XMLDocument::Init(Napi::Env env, Napi::Object exports)
  {
    DocumentBase<XMLDocument, dom::XMLDocument>::Init(env, "XMLDocument");
  }

  XMLDocument::XMLDocument(const Napi::CallbackInfo &info) : DocumentBase<XMLDocument, dom::XMLDocument>(info)
  {
  }
}
