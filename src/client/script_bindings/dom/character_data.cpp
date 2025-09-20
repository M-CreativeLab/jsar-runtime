#include "./character_data.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace dom
  {
    // static
    void CharacterData::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Add property accessors
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "data").ToLocalChecked(),
                                    DataGetter,
                                    DataSetter);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "length").ToLocalChecked(),
                                    LengthGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      // Add methods
      instanceTemplate->Set(String::NewFromUtf8(isolate, "substringData").ToLocalChecked(),
                            FunctionTemplate::New(isolate, SubstringData));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "appendData").ToLocalChecked(),
                            FunctionTemplate::New(isolate, AppendData));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "insertData").ToLocalChecked(),
                            FunctionTemplate::New(isolate, InsertData));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "deleteData").ToLocalChecked(),
                            FunctionTemplate::New(isolate, DeleteData));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "replaceData").ToLocalChecked(),
                            FunctionTemplate::New(isolate, ReplaceData));
    }

    // static
    Local<Object> CharacterData::NewInstance(Isolate *isolate, std::shared_ptr<::dom::CharacterData> nativeCharacterData)
    {
      EscapableHandleScope scope(isolate);

      if (nativeCharacterData == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      return scope.Escape(scripting_base::ObjectWrap<CharacterData, ::dom::CharacterData, Node>::NewInstance(isolate, nativeCharacterData).As<Object>());
    }

    // static
    Local<Function> CharacterData::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<CharacterData, ::dom::CharacterData, Node>::Initialize(isolate);
    }

    CharacterData::CharacterData(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<CharacterData, ::dom::CharacterData, Node>(isolate, args)
    {
    }

    // Property getters and setters

    // static
    void CharacterData::DataGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      CharacterData *characterData = scripting_base::ObjectWrap<CharacterData, ::dom::CharacterData, Node>::Unwrap(info.This());
      if (characterData == nullptr || characterData->inner() == nullptr)
      {
        info.GetReturnValue().SetEmptyString();
        return;
      }

      string data = characterData->inner()->data();
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, data.c_str()).ToLocalChecked());
    }

    // static
    void CharacterData::DataSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      CharacterData *characterData = scripting_base::ObjectWrap<CharacterData, ::dom::CharacterData, Node>::Unwrap(info.This());
      if (characterData == nullptr || characterData->inner() == nullptr)
      {
        return;
      }

      String::Utf8Value utf8Value(isolate, value);
      characterData->inner()->setData(string(*utf8Value));
    }

    // static
    void CharacterData::LengthGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      CharacterData *characterData = scripting_base::ObjectWrap<CharacterData, ::dom::CharacterData, Node>::Unwrap(info.This());
      if (characterData == nullptr || characterData->inner() == nullptr)
      {
        info.GetReturnValue().Set(Integer::New(isolate, 0));
        return;
      }

      int length = static_cast<int>(characterData->inner()->data().length());
      info.GetReturnValue().Set(Integer::New(isolate, length));
    }

    // Methods

    // static
    void CharacterData::SubstringData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "substringData requires 2 arguments").ToLocalChecked()));
        return;
      }

      CharacterData *characterData = scripting_base::ObjectWrap<CharacterData, ::dom::CharacterData, Node>::Unwrap(info.This());
      if (characterData == nullptr || characterData->inner() == nullptr)
      {
        info.GetReturnValue().SetEmptyString();
        return;
      }

      int offset = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
      int count = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);

      string data = characterData->inner()->data();
      if (offset < 0 || offset >= static_cast<int>(data.length()))
      {
        info.GetReturnValue().SetEmptyString();
        return;
      }

      int endPos = min(offset + count, static_cast<int>(data.length()));
      string substring = data.substr(offset, endPos - offset);
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, substring.c_str()).ToLocalChecked());
    }

    // static
    void CharacterData::AppendData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        return;
      }

      CharacterData *characterData = scripting_base::ObjectWrap<CharacterData, ::dom::CharacterData, Node>::Unwrap(info.This());
      if (characterData == nullptr || characterData->inner() == nullptr)
      {
        return;
      }

      String::Utf8Value utf8Value(isolate, info[0]);
      string currentData = characterData->inner()->data();
      characterData->inner()->setData(currentData + string(*utf8Value));
    }

    // static
    void CharacterData::InsertData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "insertData requires 2 arguments").ToLocalChecked()));
        return;
      }

      CharacterData *characterData = scripting_base::ObjectWrap<CharacterData, ::dom::CharacterData, Node>::Unwrap(info.This());
      if (characterData == nullptr || characterData->inner() == nullptr)
      {
        return;
      }

      int offset = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
      String::Utf8Value utf8Value(isolate, info[1]);

      string data = characterData->inner()->data();
      if (offset >= 0 && offset <= static_cast<int>(data.length()))
      {
        data.insert(offset, string(*utf8Value));
        characterData->inner()->setData(data);
      }
    }

    // static
    void CharacterData::DeleteData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "deleteData requires 2 arguments").ToLocalChecked()));
        return;
      }

      CharacterData *characterData = scripting_base::ObjectWrap<CharacterData, ::dom::CharacterData, Node>::Unwrap(info.This());
      if (characterData == nullptr || characterData->inner() == nullptr)
      {
        return;
      }

      int offset = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
      int count = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);

      string data = characterData->inner()->data();
      if (offset >= 0 && offset < static_cast<int>(data.length()))
      {
        int endPos = min(offset + count, static_cast<int>(data.length()));
        data.erase(offset, endPos - offset);
        characterData->inner()->setData(data);
      }
    }

    // static
    void CharacterData::ReplaceData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 3)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "replaceData requires 3 arguments").ToLocalChecked()));
        return;
      }

      CharacterData *characterData = scripting_base::ObjectWrap<CharacterData, ::dom::CharacterData, Node>::Unwrap(info.This());
      if (characterData == nullptr || characterData->inner() == nullptr)
      {
        return;
      }

      int offset = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
      int count = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
      String::Utf8Value utf8Value(isolate, info[2]);

      string data = characterData->inner()->data();
      if (offset >= 0 && offset < static_cast<int>(data.length()))
      {
        int endPos = min(offset + count, static_cast<int>(data.length()));
        data.replace(offset, endPos - offset, string(*utf8Value));
        characterData->inner()->setData(data);
      }
    }
  }
}