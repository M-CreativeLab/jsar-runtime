#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/character_data.hpp>
#include "./node.hpp"

namespace script_bindings
{
  namespace dom_bindings
  {
    /**
     * CharacterData wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps dom::CharacterData objects for use in V8 JavaScript execution contexts.
     * It provides the standard DOM CharacterData interface including properties like data,
     * length and methods like substringData, appendData, etc.
     */
    class CharacterData : public scripting_base::ObjectWrap<CharacterData, ::dom::CharacterData, Node>
    {
    public:
      /**
       * The name of the CharacterData class for V8.
       */
      static std::string Name()
      {
        return "CharacterData";
      }

      /**
       * Configure the V8 function template with CharacterData properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 CharacterData instance from a native dom::CharacterData.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::dom::CharacterData> nativeCharacterData);

      /**
       * Initialize the CharacterData class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

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