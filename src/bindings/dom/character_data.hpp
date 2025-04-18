#pragma once

#include <vector>
#include <napi.h>
#include <client/dom/character_data.hpp>

#include "./node-inl.hpp"

namespace dombinding
{
  template <typename ObjectType, typename NodeType>
  class CharacterDataBase : public NodeBase<ObjectType, NodeType>
  {
  public:
    static vector<Napi::ClassPropertyDescriptor<ObjectType>> GetClassProperties(Napi::Env env)
    {
      using T = CharacterDataBase<ObjectType, NodeType>;
      auto props = NodeBase<ObjectType, NodeType>::GetClassProperties(env);
      auto added = std::vector<Napi::ClassPropertyDescriptor<ObjectType>>(
          {
              T::InstanceAccessor("data", &T::DataGetter, &T::DataSetter, napi_default_jsproperty),
              T::InstanceAccessor("length", &T::LengthGetter, nullptr, napi_default_jsproperty),
              T::InstanceMethod("after", &T::After),
              T::InstanceMethod("appendData", &T::AppendData),
              T::InstanceMethod("before", &T::Before),
              T::InstanceMethod("deleteData", &T::DeleteData),
              T::InstanceMethod("insertData", &T::InsertData),
              T::InstanceMethod("remove", &T::Remove),
              T::InstanceMethod("replaceData", &T::ReplaceData),
              T::InstanceMethod("replaceWith", &T::ReplaceWith),
              T::InstanceMethod("substringData", &T::SubstringData),
          });
      props.insert(props.end(), added.begin(), added.end());
      return props;
    }

  public:
    CharacterDataBase(const Napi::CallbackInfo &info)
        : NodeBase<ObjectType, NodeType>(info)
    {
    }

  private:
    Napi::Value DataGetter(const Napi::CallbackInfo &info);
    void DataSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value LengthGetter(const Napi::CallbackInfo &info);
    Napi::Value After(const Napi::CallbackInfo &info);
    Napi::Value AppendData(const Napi::CallbackInfo &info);
    Napi::Value Before(const Napi::CallbackInfo &info);
    Napi::Value DeleteData(const Napi::CallbackInfo &info);
    Napi::Value InsertData(const Napi::CallbackInfo &info);
    Napi::Value Remove(const Napi::CallbackInfo &info);
    Napi::Value ReplaceData(const Napi::CallbackInfo &info);
    Napi::Value ReplaceWith(const Napi::CallbackInfo &info);
    Napi::Value SubstringData(const Napi::CallbackInfo &info);
  };

  class CharacterData : public CharacterDataBase<CharacterData, dom::CharacterData>
  {
    using CharacterDataBase::CharacterDataBase;
    friend class NodeBase<CharacterData, dom::CharacterData>;

  public:
    static void Init(Napi::Env env);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
