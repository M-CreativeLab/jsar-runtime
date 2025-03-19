#include "./text.hpp"
#include "./character_data.hpp"

namespace dombinding
{
  using namespace std;
  using namespace Napi;

  thread_local Napi::FunctionReference *Text::constructor;
  void Text::Init(Napi::Env env)
  {
#define MODULE_NAME "Text"
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, MODULE_NAME, props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set(MODULE_NAME, func);
#undef MODULE_NAME
  }

  vector<Napi::ClassPropertyDescriptor<Text>> Text::GetClassProperties(Napi::Env env)
  {
    auto props = CharacterDataBase<Text, dom::Text>::GetClassProperties(env);
    auto added = vector<Napi::ClassPropertyDescriptor<Text>>(
        {
            InstanceAccessor("assignedSlot", &Text::AssignedSlotGetter, nullptr),
            InstanceAccessor("wholeText", &Text::WholeTextGetter, nullptr),
            InstanceMethod("splitText", &Text::SplitText),
        });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  Napi::Value Text::NewInstance(Napi::Env env, std::shared_ptr<dom::Node> nodeImpl)
  {
    auto textImpl = dynamic_pointer_cast<dom::Text>(nodeImpl);
    if (textImpl != nullptr)
      return Text::FromImpl(env, textImpl);

    auto characterDataImpl = dynamic_pointer_cast<dom::CharacterData>(nodeImpl);
    if (characterDataImpl != nullptr)
      return CharacterData::FromImpl(env, characterDataImpl);

    // Fallback to the `Null` if the TEXT_NODE failed to create in the above cases.
    return env.Null();
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
