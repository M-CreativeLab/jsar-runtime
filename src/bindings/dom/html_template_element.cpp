#include <client/html/html_template_element.hpp>
#include <client/dom/document_fragment.hpp>

#include "./html_template_element.hpp"
#include "./node-inl.hpp"

namespace dombinding
{
  using namespace std;

  thread_local Napi::FunctionReference *HTMLTemplateElement::constructor;
  void HTMLTemplateElement::Init(Napi::Env env)
  {
#define MODULE_NAME "HTMLTemplateElement"
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, MODULE_NAME, props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set(MODULE_NAME, func);
#undef MODULE_NAME
  }

  vector<Napi::ClassPropertyDescriptor<HTMLTemplateElement>> HTMLTemplateElement::GetClassProperties(Napi::Env env)
  {
    using T = HTMLTemplateElement;
    auto props = HTMLElementBase<HTMLTemplateElement, dom::HTMLTemplateElement>::GetClassProperties(env);
    auto added = vector<Napi::ClassPropertyDescriptor<HTMLTemplateElement>>(
        {
            T::InstanceAccessor("content", &T::ContentGetter, nullptr),
        });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  Napi::Value HTMLTemplateElement::ContentGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Node::NewInstance(env, this->node->getContent());
  }
}
