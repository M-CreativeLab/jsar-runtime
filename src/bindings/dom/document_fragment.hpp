#pragma once

#include <memory>
#include <vector>
#include <napi.h>
#include <client/dom/document_fragment.hpp>

#include "./node.hpp"

namespace dombinding
{
  class DocumentFragment final : public NodeBase<DocumentFragment, dom::DocumentFragment>
  {
    using NodeBase<DocumentFragment, dom::DocumentFragment>::NodeBase;
    friend class NodeBase<DocumentFragment, dom::DocumentFragment>;

  public:
    static void Init(Napi::Env env);
    static std::vector<Napi::ClassPropertyDescriptor<DocumentFragment>> GetClassProperties(Napi::Env env);
    static Napi::Value NewInstance(Napi::Env env, std::shared_ptr<dom::Node> node);

  private:
    Napi::Value ChildElementCountGetter(const Napi::CallbackInfo &info);
    Napi::Value ChildrenGetter(const Napi::CallbackInfo &info);
    Napi::Value FirstElementChildGetter(const Napi::CallbackInfo &info);
    Napi::Value LastElementChildGetter(const Napi::CallbackInfo &info);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
