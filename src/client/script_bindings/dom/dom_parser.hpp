#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/dom_parser.hpp>

namespace script_bindings
{
  namespace dom_bindings
  {
    class DOMParser;
    using DOMParserBase = scripting_base::ObjectWrap<DOMParser, dom::DOMParser>;

    /**
   * DOMParser wrapper for V8 objects using scripting_base::ObjectWrap.
   * 
   * This class wraps dom::DOMParser objects for use in V8 JavaScript execution contexts.
   * It provides the standard DOM DOMParser interface for parsing XML/HTML from strings.
   */
    class DOMParser : public DOMParserBase
    {
      using DOMParserBase::ObjectWrap;

    public:
      /**
     * The name of the DOMParser class for V8.
     */
      static std::string Name()
      {
        return "DOMParser";
      }

      /**
     * Configure the V8 function template with DOMParser properties and methods.
     */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
     * Create a new V8 DOMParser instance from a native dom::DOMParser.
     */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<dom::DOMParser> nativeParser);

      /**
     * Initialize the DOMParser class and register it with V8.
     */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      DOMParser(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Core methods
      static void ParseFromString(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}