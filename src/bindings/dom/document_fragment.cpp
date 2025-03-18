#include "./document_fragment.hpp"
#include "./node-inl.hpp"

namespace dombinding
{
  using namespace std;

  thread_local Napi::FunctionReference *DocumentFragment::constructor;
  void DocumentFragment::Init(Napi::Env env)
  {
#define MODULE_NAME "DocumentFragment"
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, MODULE_NAME, props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set(MODULE_NAME, func);
#undef MODULE_NAME
  }

  vector<Napi::ClassPropertyDescriptor<DocumentFragment>> DocumentFragment::GetClassProperties(Napi::Env env)
  {
    auto props = NodeBase<DocumentFragment, dom::DocumentFragment>::GetClassProperties(env);
    auto added = std::vector<Napi::ClassPropertyDescriptor<DocumentFragment>>(
        {
            InstanceAccessor("childElementCount", &DocumentFragment::ChildElementCountGetter, nullptr),
            InstanceAccessor("children", &DocumentFragment::ChildrenGetter, nullptr),
            InstanceAccessor("firstElementChild", &DocumentFragment::FirstElementChildGetter, nullptr),
            InstanceAccessor("lastElementChild", &DocumentFragment::LastElementChildGetter, nullptr),
        });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  Napi::Value DocumentFragment::NewInstance(Napi::Env env, std::shared_ptr<dom::Node> node)
  {
    auto fragment = std::dynamic_pointer_cast<dom::DocumentFragment>(node);
    if (fragment != nullptr)
      return DocumentFragment::FromImpl(env, fragment);

    // Fallback to the `Null` if the DOCUMENT_FRAGMENT_NODE failed to create in the above cases.
    return env.Null();
  }

  Napi::Value DocumentFragment::ChildElementCountGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, this->node->childElementCount());
  }

  Napi::Value DocumentFragment::ChildrenGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::TypeError::New(env, "Failed to get 'children' property: not implemented")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  Napi::Value DocumentFragment::FirstElementChildGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::TypeError::New(env, "Failed to get 'firstElementChild' property: not implemented")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  Napi::Value DocumentFragment::LastElementChildGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::TypeError::New(env, "Failed to get 'lastElementChild' property: not implemented")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
}
