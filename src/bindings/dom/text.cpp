#include "./text.hpp"

namespace dombinding
{
  using namespace std;

  thread_local Napi::FunctionReference *Text::constructor;
  void Text::Init(Napi::Env env)
  {
#define MODULE_NAME "Text"
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, MODULE_NAME, props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set(MODULE_NAME, func);
#undef MODULE_NAME
  }

  vector<Napi::ClassPropertyDescriptor<Text>> Text::GetClassProperties()
  {
    auto props = NodeBase<Text, dom::Text>::GetClassProperties();
    auto added = vector<Napi::ClassPropertyDescriptor<Text>>(
        {
            InstanceAccessor("assignedSlot", &Text::AssignedSlotGetter, nullptr),
            InstanceAccessor("wholeText", &Text::WholeTextGetter, nullptr),
            InstanceMethod("splitText", &Text::SplitText),
        });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  Text::Text(const Napi::CallbackInfo &info)
      : CharacterDataBase(info)
  {
  }

  Napi::Value Text::AssignedSlotGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  Napi::Value Text::WholeTextGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto textNode = this->node;
    assert(textNode != nullptr);
    return Napi::String::New(env, textNode->wholeText());
  }

  Napi::Value Text::SplitText(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto textNode = this->node;
    assert(textNode != nullptr);

    if (info.Length() < 1)
    {
      Napi::Error::New(env, "splitText requires at least 1 argument").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::Error::New(env, "splitText requires a number as the first argument").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto offset = info[0].As<Napi::Number>().Int32Value();
    auto newText = textNode->splitText(offset);
    return NodeBase<Text, dom::Text>::FromImpl(env, move(newText));
  }
}
