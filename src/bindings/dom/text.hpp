#pragma once

#include <vector>
#include <napi.h>
#include <client/dom/text.hpp>

#include "./character_data-inl.hpp"

namespace dombinding
{
  class Text : public CharacterDataBase<Text, dom::Text>
  {
    using CharacterDataBase::CharacterDataBase;
    friend class NodeBase<Text, dom::Text>;

  public:
    static void Init(Napi::Env env);
    static std::vector<Napi::ClassPropertyDescriptor<Text>> GetClassProperties();

  public:
    Text(const Napi::CallbackInfo &info);

  private:
    Napi::Value AssignedSlotGetter(const Napi::CallbackInfo &info);
    Napi::Value WholeTextGetter(const Napi::CallbackInfo &info);
    Napi::Value SplitText(const Napi::CallbackInfo &info);

  private:
    thread_local static Napi::FunctionReference *constructor;
  };
}
