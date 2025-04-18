#pragma once

#include "./character_data.hpp"

namespace dombinding
{
  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::DataGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    std::string data = this->node->data();
    return Napi::String::New(env, data);
  }

  template <typename ObjectType, typename NodeType>
  void CharacterDataBase<ObjectType, NodeType>::DataSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (!value.IsString() && !value.IsNull())
    {
      Napi::TypeError::New(env, "Failed to set 'data': Invalid value").ThrowAsJavaScriptException();
      return;
    }

    std::string newData = "";
    if (value.IsString())
      newData = value.As<Napi::String>().Utf8Value();
    this->node->data() = newData;
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::LengthGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    size_t length = this->node->length();
    return Napi::Number::New(env, length);
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::After(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "Failed to call 'after' method: 1 argument required, but only 0 present.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    // After should insert from the end to the beginning.
    for (size_t i = info.Length(); i > 0; i--)
    {
      auto nodeValue = info[i - 1];
      if (nodeValue.IsString())
      {
        std::string text = nodeValue.As<Napi::String>().Utf8Value();
        this->node->after(text);
      }
      else
      {
        auto node = Node::GetImpl(nodeValue);
        if (node != nullptr)
        {
          this->node->after(node);
        }
        else
        {
          Napi::TypeError::New(env, "Failed to call 'after' method: invalid argument").ThrowAsJavaScriptException();
          return env.Undefined();
        }
      }
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::AppendData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "Failed to call 'appendData' method: 1 argument required, but only 0 present.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (!info[0].IsString())
    {
      Napi::TypeError::New(env, "Failed to call 'appendData' method: invalid argument").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::string data = info[0].As<Napi::String>().Utf8Value();
    this->node->appendData(data);
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::Before(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "Failed to call 'before' method: 1 argument required, but only 0 present.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    for (size_t i = 0; i < info.Length(); i++)
    {
      auto nodeValue = info[i];
      if (nodeValue.IsString())
      {
        std::string text = nodeValue.As<Napi::String>().Utf8Value();
        this->node->before(text);
      }
      else
      {
        auto node = Node::GetImpl(nodeValue);
        if (node != nullptr)
        {
          this->node->before(node);
        }
        else
        {
          Napi::TypeError::New(env, "Failed to call 'before' method: invalid argument").ThrowAsJavaScriptException();
          return env.Undefined();
        }
      }
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::DeleteData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "Failed to call 'deleteData' method: 2 arguments required, but only 0 present.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (!info[0].IsNumber() || !info[1].IsNumber())
    {
      Napi::TypeError::New(env, "Failed to call 'deleteData' method: invalid arguments").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    size_t offset = info[0].As<Napi::Number>().Int32Value();
    size_t count = info[1].As<Napi::Number>().Int32Value();
    this->node->deleteData(offset, count);
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::InsertData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "Failed to call 'insertData' method: 2 arguments required, but only 0 present.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (!info[0].IsNumber() || !info[1].IsString())
    {
      Napi::TypeError::New(env, "Failed to call 'insertData' method: invalid arguments").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    size_t offset = info[0].As<Napi::Number>().Int32Value();
    std::string data = info[1].As<Napi::String>().Utf8Value();
    this->node->insertData(offset, data);
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::Remove(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    this->node->remove(); // Just remove the underlying node but not the JS object.
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value CharacterDataBase<ObjectType, NodeType>::ReplaceData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "Failed to call 'replaceData' method: 3 arguments required, but only 0 present.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (!info[0].IsNumber() || !info[1].IsNumber() || !info[2].IsString())
    {
      Napi::TypeError::New(env, "Failed to call 'replaceData' method: invalid arguments").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    size_t offset = info[0].As<Napi::Number>().Int32Value();
    size_t count = info[1].As<Napi::Number>().Int32Value();
    std::string data = info[2].As<Napi::String>().Utf8Value();
    this->node->replaceData(offset, count, data);
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
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "Failed to call 'substringData' method: 2 arguments required, but only 0 present.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (!info[0].IsNumber() || !info[1].IsNumber())
    {
      Napi::TypeError::New(env, "Failed to call 'substringData' method: invalid arguments").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    size_t offset = info[0].As<Napi::Number>().Int32Value();
    size_t count = info[1].As<Napi::Number>().Int32Value();
    std::string data = this->node->substringData(offset, count);
    return Napi::String::New(env, data);
  }
}
