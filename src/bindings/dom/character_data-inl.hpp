#pragma once

#include "./character_data.hpp"

namespace dombinding
{
  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::DataGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::Error::New(env, "DataGetter is not implemented").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  void CharacterDataBase<ObjectType, NodeType>::DataSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::Error::New(env, "DataSetter is not implemented").ThrowAsJavaScriptException();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::LengthGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::Error::New(env, "LengthGetter is not implemented").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  void CharacterDataBase<ObjectType, NodeType>::LengthSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::Error::New(env, "LengthSetter is not implemented").ThrowAsJavaScriptException();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::After(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::AppendData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::Before(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::DeleteData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::InsertData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::Remove(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::ReplaceData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::ReplaceWith(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::SubstringData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }
}
