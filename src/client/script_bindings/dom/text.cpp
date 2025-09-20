#include "./text.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace dom
  {
    // static
    void Text::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
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

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "wholeText").ToLocalChecked(),
                                    WholeTextGetter,
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

      instanceTemplate->Set(String::NewFromUtf8(isolate, "splitText").ToLocalChecked(),
                            FunctionTemplate::New(isolate, SplitText));
    }

    // static
    Local<Object> Text::NewInstance(Isolate *isolate, std::shared_ptr<::dom::Text> nativeText)
    {
      EscapableHandleScope scope(isolate);

      if (nativeText == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      return scope.Escape(scripting_base::ObjectWrap<Text, ::dom::Text, Node>::NewInstance(isolate, nativeText).As<Object>());
    }

    // static
    Local<Function> Text::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<Text, ::dom::Text, Node>::Initialize(isolate);
    }

    Text::Text(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<Text, ::dom::Text, Node>(isolate, args)
    {
    }

    // Property getters and setters

    // static
    void Text::DataGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Text *text = scripting_base::ObjectWrap<Text, ::dom::Text, Node>::Unwrap(info.This());
      if (text == nullptr || text->inner() == nullptr)
      {
        info.GetReturnValue().SetEmptyString();
        return;
      }

      string data = text->inner()->data;
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, data.c_str()).ToLocalChecked());
    }

    // static
    void Text::DataSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Text *text = scripting_base::ObjectWrap<Text, ::dom::Text, Node>::Unwrap(info.This());
      if (text == nullptr || text->inner() == nullptr)
      {
        return;
      }

      String::Utf8Value utf8Value(isolate, value);
      text->inner()->data = string(*utf8Value);
    }

    // static
    void Text::LengthGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Text *text = scripting_base::ObjectWrap<Text, ::dom::Text, Node>::Unwrap(info.This());
      if (text == nullptr || text->inner() == nullptr)
      {
        info.GetReturnValue().Set(Integer::New(isolate, 0));
        return;
      }

      int length = static_cast<int>(text->inner()->data.length());
      info.GetReturnValue().Set(Integer::New(isolate, length));
    }

    // static
    void Text::WholeTextGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Text *text = scripting_base::ObjectWrap<Text, ::dom::Text, Node>::Unwrap(info.This());
      if (text == nullptr || text->inner() == nullptr)
      {
        info.GetReturnValue().SetEmptyString();
        return;
      }

      // For now, return the same as data. In a full implementation, this would
      // concatenate adjacent text nodes
      string wholeText = text->inner()->data;
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, wholeText.c_str()).ToLocalChecked());
    }

    // Methods

    // static
    void Text::SubstringData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "substringData requires 2 arguments").ToLocalChecked()));
        return;
      }

      Text *text = scripting_base::ObjectWrap<Text, ::dom::Text, Node>::Unwrap(info.This());
      if (text == nullptr || text->inner() == nullptr)
      {
        info.GetReturnValue().SetEmptyString();
        return;
      }

      int offset = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
      int count = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);

      string data = text->inner()->data;
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
    void Text::AppendData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        return;
      }

      Text *text = scripting_base::ObjectWrap<Text, ::dom::Text, Node>::Unwrap(info.This());
      if (text == nullptr || text->inner() == nullptr)
      {
        return;
      }

      String::Utf8Value utf8Value(isolate, info[0]);
      text->inner()->data += string(*utf8Value);
    }

    // static
    void Text::InsertData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "insertData requires 2 arguments").ToLocalChecked()));
        return;
      }

      Text *text = scripting_base::ObjectWrap<Text, ::dom::Text, Node>::Unwrap(info.This());
      if (text == nullptr || text->inner() == nullptr)
      {
        return;
      }

      int offset = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
      String::Utf8Value utf8Value(isolate, info[1]);

      string &data = text->inner()->data;
      if (offset >= 0 && offset <= static_cast<int>(data.length()))
      {
        data.insert(offset, string(*utf8Value));
      }
    }

    // static
    void Text::DeleteData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "deleteData requires 2 arguments").ToLocalChecked()));
        return;
      }

      Text *text = scripting_base::ObjectWrap<Text, ::dom::Text, Node>::Unwrap(info.This());
      if (text == nullptr || text->inner() == nullptr)
      {
        return;
      }

      int offset = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
      int count = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);

      string &data = text->inner()->data;
      if (offset >= 0 && offset < static_cast<int>(data.length()))
      {
        int endPos = min(offset + count, static_cast<int>(data.length()));
        data.erase(offset, endPos - offset);
      }
    }

    // static
    void Text::ReplaceData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 3)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "replaceData requires 3 arguments").ToLocalChecked()));
        return;
      }

      Text *text = scripting_base::ObjectWrap<Text, ::dom::Text, Node>::Unwrap(info.This());
      if (text == nullptr || text->inner() == nullptr)
      {
        return;
      }

      int offset = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
      int count = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
      String::Utf8Value utf8Value(isolate, info[2]);

      string &data = text->inner()->data;
      if (offset >= 0 && offset < static_cast<int>(data.length()))
      {
        int endPos = min(offset + count, static_cast<int>(data.length()));
        data.replace(offset, endPos - offset, string(*utf8Value));
      }
    }

    // static
    void Text::SplitText(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Text *text = scripting_base::ObjectWrap<Text, ::dom::Text, Node>::Unwrap(info.This());
      if (text == nullptr || text->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Implement splitText - should create a new Text node
      cout << "splitText called" << endl;
      info.GetReturnValue().SetNull();
    }
  }
}