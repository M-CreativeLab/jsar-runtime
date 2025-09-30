#include <iostream>
#include "./character_data.hpp"
#include "./comment.hpp"
#include "./text.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace dom_bindings
  {
    void CharacterData::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

      // Add property accessors
      InstanceReadonlyAccessor(isolate, prototype, "length", &CharacterData::LengthGetter);
      InstanceAccessor(isolate, prototype, "data", &CharacterData::DataGetter, &CharacterData::DataSetter);

      // Add methods
      InstanceMethod(isolate, prototype, "substringData", &CharacterData::SubstringData);
      InstanceMethod(isolate, prototype, "appendData", &CharacterData::AppendData);
      InstanceMethod(isolate, prototype, "insertData", &CharacterData::InsertData);
      InstanceMethod(isolate, prototype, "deleteData", &CharacterData::DeleteData);
      InstanceMethod(isolate, prototype, "replaceData", &CharacterData::ReplaceData);
    }

    Local<Object> CharacterData::NewInstance(Isolate *isolate, std::shared_ptr<::dom::CharacterData> handle)
    {
      EscapableHandleScope scope(isolate);
      assert(handle != nullptr && "CharacterData::NewInstance: handle is null");

      if (handle->isText())
        return scope.Escape(Text::NewInstance(isolate, static_pointer_cast<::dom::Text>(handle)));
      else if (handle->isComment())
        return scope.Escape(Comment::NewInstance(isolate, static_pointer_cast<::dom::Comment>(handle)));
      else
        return scope.Escape(CharacterDataBase::NewInstance(isolate, handle));
    }

    CharacterData::CharacterData(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : CharacterDataBase(isolate, args, true)
    {
    }

    // Property getters and setters

    void CharacterData::DataGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      string data = handle()->data();
      info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    data.c_str())
                                  .ToLocalChecked());
    }

    void CharacterData::DataSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      string newData = "";
      if (value->IsString())
      {
        String::Utf8Value utf8Value(isolate, value);
        newData = string(*utf8Value);
      }
      handle()->data() = newData;
    }

    void CharacterData::LengthGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      int length = static_cast<int>(handle()->data().length());
      info.GetReturnValue().Set(Integer::New(isolate, length));
    }

    // Methods

    void CharacterData::SubstringData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "substringData requires 2 arguments").ToLocalChecked()));
        return;
      }

      int offset = info[0]->Int32Value(context).FromMaybe(0);
      int count = info[1]->Int32Value(context).FromMaybe(0);

      string data = handle()->data();
      if (offset < 0 || offset >= static_cast<int>(data.length()))
      {
        info.GetReturnValue().SetEmptyString();
        return;
      }

      int endPos = min(offset + count, static_cast<int>(data.length()));
      string substring = data.substr(offset, endPos - offset);
      info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    substring.c_str())
                                  .ToLocalChecked());
    }

    void CharacterData::AppendData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Failed to call 'appendData' method: 1 argument required, but only 0 present.")
            .ToLocalChecked()));
        return;
      }

      if (!info[0]->IsString())
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Failed to call 'appendData' method: invalid argument").ToLocalChecked()));
        return;
      }

      Local<String> dataString = info[0].As<String>();
      String::Utf8Value utf8Value(isolate, dataString);
      handle()->appendData(string(*utf8Value));
      return info.GetReturnValue().SetUndefined();
    }

    void CharacterData::InsertData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Failed to call 'insertData' method: 2 arguments required, but only 0 present.")
            .ToLocalChecked()));
        return;
      }

      if (!info[0]->IsNumber() || !info[1]->IsString())
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Failed to call 'insertData' method: invalid arguments").ToLocalChecked()));
        return;
      }

      int offset = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
      String::Utf8Value utf8Value(isolate, info[1]);
      handle()->insertData(offset, string(*utf8Value));
      return info.GetReturnValue().SetUndefined();
    }

    void CharacterData::DeleteData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Failed to call 'deleteData' method: 2 arguments required, but only 0 present.")
            .ToLocalChecked()));
        return;
      }

      if (!info[0]->IsNumber() || !info[1]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Failed to call 'insertData' method: invalid arguments").ToLocalChecked()));
        return;
      }

      int offset = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
      int count = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
      handle()->deleteData(offset, count);
      return info.GetReturnValue().SetUndefined();
    }

    void CharacterData::ReplaceData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 3)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Failed to call 'replaceData' method: 3 arguments required, but only 0 present.")
            .ToLocalChecked()));
        return;
      }

      if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsString())
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Failed to call 'replaceData' method: invalid arguments").ToLocalChecked()));
        return;
      }

      int offset = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
      int count = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
      String::Utf8Value utf8Value(isolate, info[2]);
      handle()->replaceData(offset, count, string(*utf8Value));
      return info.GetReturnValue().SetUndefined();
    }
  }
}
