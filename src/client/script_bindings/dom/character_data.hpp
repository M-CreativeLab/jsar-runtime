#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/character_data.hpp>
#include "./node.hpp"

namespace script_bindings
{
  namespace dom_bindings
  {
    class CharacterData;
    using CharacterDataBase = scripting_base::ObjectWrap<CharacterData, ::dom::CharacterData, Node>;

    /**
     * CharacterData wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps dom::CharacterData objects for use in V8 JavaScript execution contexts.
     * It provides the standard DOM CharacterData interface including properties like data,
     * length and methods like substringData, appendData, etc.
     */
    class CharacterData : public CharacterDataBase
    {
    public:
      static std::string Name()
      {
        return "CharacterData";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::dom::CharacterData> handle);

    public:
      CharacterData(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters and setters
      static void DataGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void DataSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void LengthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

      // Methods
      static void SubstringData(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void AppendData(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void InsertData(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void DeleteData(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void ReplaceData(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}