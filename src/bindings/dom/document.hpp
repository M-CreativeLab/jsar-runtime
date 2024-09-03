#pragma once

#include <memory>
#include <napi.h>
#include "client/dom/document.hpp"

using namespace std;

namespace dombinding
{
  template <typename T, typename DocumentType>
  class BaseDocument : public Napi::ObjectWrap<T>
  {
  public:
    static void Init(Napi::Env env, const std::string &name)
    {
      Napi::Function func = T::DefineClass(env, name.c_str(),
                                           {
                                               T::InstanceMethod("adoptNode", &T::AdoptNode),
                                               T::InstanceMethod("append", &T::Append),
                                               T::InstanceMethod("close", &T::Close),
                                               T::InstanceMethod("createAttribute", &T::CreateAttribute),
                                               T::InstanceMethod("createAttributeNS", &T::CreateAttributeNS),
                                               T::InstanceMethod("createCDATASection", &T::CreateCDATASection),
                                               T::InstanceMethod("createComment", &T::CreateComment),
                                               T::InstanceMethod("createDocumentFragment", &T::CreateDocumentFragment),
                                               T::InstanceMethod("createElement", &T::CreateElement),
                                               T::InstanceMethod("createElementNS", &T::CreateElementNS),
                                           });
      T::constructor = new Napi::FunctionReference();
      *T::constructor = Napi::Persistent(func);
      env.Global().Set(name, func);
    }
    /**
     * Create a new JavaScript instance of the document
     * 
     * @param env The env
     * @param internalDoc The internal document
     */
    static Napi::Object NewInstance(Napi::Env env, shared_ptr<DocumentType> internalDoc)
    {
      Napi::EscapableHandleScope scope(env);
      Napi::Object obj = T::constructor->New({});
      T *instance = Napi::ObjectWrap<T>::Unwrap(obj);
      instance->internalDoc = internalDoc;
      return scope.Escape(obj).ToObject();
    }

  public:
    BaseDocument(const Napi::CallbackInfo &info) : Napi::ObjectWrap<T>(info)
    {
    }
    ~BaseDocument()
    {
    }

  protected:
    Napi::Value AdoptNode(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
    Napi::Value Append(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
    Napi::Value Close(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
    Napi::Value CreateAttribute(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
    Napi::Value CreateAttributeNS(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
    Napi::Value CreateCDATASection(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
    Napi::Value CreateComment(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
    Napi::Value CreateDocumentFragment(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
    Napi::Value CreateElement(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
    Napi::Value CreateElementNS(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }

  protected:
    shared_ptr<DocumentType> internalDoc;
  };

  class Document : public BaseDocument<Document, dom::HTMLDocument>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);

  public:
    Document(const Napi::CallbackInfo &info);

  public:
    static Napi::FunctionReference *constructor;
  };

  class XMLDocument : public BaseDocument<XMLDocument, dom::XMLDocument>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);

  public:
    XMLDocument(const Napi::CallbackInfo &info);

  public:
    static Napi::FunctionReference *constructor;
  };
}
