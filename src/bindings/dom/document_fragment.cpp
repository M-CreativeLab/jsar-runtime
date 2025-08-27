#include "./document_fragment.hpp"
#include "./node-inl.hpp"
#include "./element.hpp"
#include "./node_list-inl.hpp"

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

        // ParentNode interface methods
        InstanceMethod("querySelector", &DocumentFragment::QuerySelector),
        InstanceMethod("querySelectorAll", &DocumentFragment::QuerySelectorAll),
        InstanceMethod("append", &DocumentFragment::Append),
        InstanceMethod("prepend", &DocumentFragment::Prepend),
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

    auto children = this->node->children();
    auto list = std::make_unique<dom::NodeList<dom::Element>>(std::move(children));
    return NodeList::NewInstance(env, std::move(list));
  }

  Napi::Value DocumentFragment::FirstElementChildGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto element = this->node->firstElementChild();
    return element != nullptr
             ? Element::NewInstance(env, element)
             : env.Null();
  }

  Napi::Value DocumentFragment::LastElementChildGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto element = this->node->lastElementChild();
    return element != nullptr
             ? Element::NewInstance(env, element)
             : env.Null();
  }

  Napi::Value DocumentFragment::QuerySelector(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(
        env, "Failed to execute 'querySelector' on 'DocumentFragment': 1 argument required, but only 0 present.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto selectors = info[0].ToString().Utf8Value();
    try
    {
      std::shared_ptr<dom::Element> element = this->node->querySelector(selectors);
      return element == nullptr ? env.Null() : Element::NewInstance(env, element);
    }
    catch (const std::exception &e)
    {
      auto msg = "Failed to execute 'querySelector' on 'DocumentFragment': '" + selectors + "' is not a valid selector.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }

  Napi::Value DocumentFragment::QuerySelectorAll(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(
        env, "Failed to execute 'querySelectorAll' on 'DocumentFragment': 1 argument required, but only 0 present.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto selectors = info[0].ToString().Utf8Value();
    try
    {
      auto list = std::make_unique<dom::NodeList<dom::Element>>(this->node->querySelectorAll(selectors));
      return NodeList::NewInstance(env, std::move(list));
    }
    catch (const std::exception &e)
    {
      auto msg = "Failed to execute 'querySelectorAll' on 'DocumentFragment': '" + selectors + "' is not a valid selector.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }

  Napi::Value DocumentFragment::Append(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    std::vector<std::shared_ptr<dom::Node>> nodes;

    for (size_t i = 0; i < info.Length(); i++)
    {
      auto arg = info[i];
      if (arg.IsString())
      {
        // Create a text node from string
        auto document = this->node->getOwnerDocumentChecked();
        auto textNode = document.createTextNode(arg.ToString().Utf8Value());
        nodes.push_back(textNode);
      }
      else if (arg.IsObject())
      {
        // Try to extract node from object
        auto obj = arg.ToObject();
        if (obj.Has(NODE_IMPL_FIELD))
        {
          auto jsNodeImplExternal = obj.Get(NODE_IMPL_FIELD).As<Napi::External<NodeContainer<dom::Node>>>();
          auto nodeImpl = jsNodeImplExternal.Data()->node;
          nodes.push_back(nodeImpl);
        }
      }
    }

    this->node->append(nodes);
    return env.Undefined();
  }

  Napi::Value DocumentFragment::Prepend(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    std::vector<std::shared_ptr<dom::Node>> nodes;

    for (size_t i = 0; i < info.Length(); i++)
    {
      auto arg = info[i];
      if (arg.IsString())
      {
        // Create a text node from string
        auto document = this->node->getOwnerDocumentChecked();
        auto textNode = document.createTextNode(arg.ToString().Utf8Value());
        nodes.push_back(textNode);
      }
      else if (arg.IsObject())
      {
        // Try to extract node from object
        auto obj = arg.ToObject();
        if (obj.Has(NODE_IMPL_FIELD))
        {
          auto jsNodeImplExternal = obj.Get(NODE_IMPL_FIELD).As<Napi::External<NodeContainer<dom::Node>>>();
          auto nodeImpl = jsNodeImplExternal.Data()->node;
          nodes.push_back(nodeImpl);
        }
      }
    }

    this->node->prepend(nodes);
    return env.Undefined();
  }
}
