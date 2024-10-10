#include "./document.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *Document::constructor;
  void Document::Init(Napi::Env env, Napi::Object exports)
  {
    DocumentBase<Document, dom::HTMLDocument>::Init(env, "Document");
    {
      auto tid = std::this_thread::get_id();
      std::cerr << "Document::Init() => " << tid << std::endl;
    }
  }

  Document *Document::GetCurrent(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    auto tid = std::this_thread::get_id();
    std::cerr << "GetCurrent() => " << tid << std::endl;

    auto document = env.Global().Get("document");
    if (document.IsObject())
    {
      auto objectObject = document.ToObject();
      if (objectObject.InstanceOf(Document::constructor->Value()))
      {
        return Document::Unwrap(objectObject);
      }
    }
    return nullptr;
  }

  Document::Document(const Napi::CallbackInfo &info) : DocumentBase<Document, dom::HTMLDocument>(info)
  {
  }

  thread_local Napi::FunctionReference *XMLDocument::constructor;
  void XMLDocument::Init(Napi::Env env, Napi::Object exports)
  {
    DocumentBase<XMLDocument, dom::XMLDocument>::Init(env, "XMLDocument");
  }

  XMLDocument::XMLDocument(const Napi::CallbackInfo &info) : DocumentBase<XMLDocument, dom::XMLDocument>(info)
  {
  }
}
