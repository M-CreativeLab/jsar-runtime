#pragma once

#include <memory>
#include <napi.h>
#include "client/dom/node.hpp"

namespace dombinding
{
  class Node : public Napi::ObjectWrap<Node>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    Node(const Napi::CallbackInfo &info);
    ~Node();

  private:
    Napi::Value AppendChild(const Napi::CallbackInfo &info);
    Napi::Value CloneNode(const Napi::CallbackInfo &info);
    Napi::Value CompareDocumentPosition(const Napi::CallbackInfo &info);
    Napi::Value Contains(const Napi::CallbackInfo &info);
    Napi::Value GetRootNode(const Napi::CallbackInfo &info);
    Napi::Value HasChildNodes(const Napi::CallbackInfo &info);
    Napi::Value InsertBefore(const Napi::CallbackInfo &info);

  public:
    static Napi::FunctionReference *constructor;
  };
}
