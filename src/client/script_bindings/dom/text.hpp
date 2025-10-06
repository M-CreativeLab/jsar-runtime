#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/text.hpp>
#include "./character_data.hpp"

namespace script_bindings
{
  namespace dom_bindings
  {
    class Text;
    using TextBase = scripting_base::ObjectWrap<Text, ::dom::Text, CharacterData>;

    /**
     * Text wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps dom::Text objects for use in V8 JavaScript execution contexts.
     * It provides the standard DOM Text interface including properties like data,
     * length and methods like substringData, appendData, etc.
     */
    class Text : public TextBase
    {
    public:
      static std::string Name()
      {
        return "Text";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::dom::Text> nativeText);

    public:
      Text(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters and setters
      static void WholeTextGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

      // Methods
      static void SplitText(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}