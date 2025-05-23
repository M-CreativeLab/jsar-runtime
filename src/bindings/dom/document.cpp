#include "./document-inl.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *Document::constructor;
  void Document::Init(Napi::Env env, Napi::Object exports)
  {
    DocumentBase<Document, dom::HTMLDocument>::Init(env, "Document");
  }

  Document *Document::GetCurrent(Napi::Env env)
  {
    Napi::HandleScope scope(env);
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

  Napi::Value Document::NewInstance(Napi::Env env, shared_ptr<dom::Node> node)
  {
    if (node == nullptr)
      return env.Null();

    auto document = dynamic_pointer_cast<dom::Document>(node);
    assert(document != nullptr && "invalid Document.");

    if (document->documentType == dom::DocumentType::kHTML)
    {
      auto htmlDocument = dynamic_pointer_cast<dom::HTMLDocument>(document);
      assert(htmlDocument != nullptr && "invalid HTMLDocument.");
      return DocumentBase<Document, dom::HTMLDocument>::NewInstance(env, htmlDocument);
    }
    else if (document->documentType == dom::DocumentType::kXML)
    {
      auto xmlDocument = dynamic_pointer_cast<dom::XMLDocument>(document);
      assert(xmlDocument != nullptr && "invalid XMLDocument.");
      return XMLDocument::NewInstance(env, xmlDocument);
    }

    // Unreachable
    assert(false && "invalid document type.");
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
