#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/dom/node.hpp>
#include <client/dom/document.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace dom_bindings
    {
      class Document;
      using DocumentBase = scripting_base::ObjectWrap<Document, ::dom::Document, Node>;

      /**
     * Document wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps dom::Document objects for use in V8 JavaScript execution contexts.
     * It provides the standard DOM Document interface including methods like createElement,
     * getElementById, querySelector, etc.
     */
      class Document : public DocumentBase
      {
        using DocumentBase::ObjectWrap;

      public:
        /**
       * The name of the Document class for V8.
       */
        static std::string Name()
        {
          return "Document";
        }

        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
        static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::dom::Document> nativeDocument);

      public:
        Document(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

      private:
        // Property getters
        void CompatModeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void ContentTypeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void DesignModeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void DesignModeSetter(v8::Local<v8::Value> value,
                              const v8::PropertyCallbackInfo<void> &info);

        void DocumentURIGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void DocumentElementGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void BodyGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void BodySetter(v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void> &info);
        void HeadGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void TitleGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void TitleSetter(v8::Local<v8::Value> value,
                         const v8::PropertyCallbackInfo<void> &info);

        // Methods
        void Append(const v8::FunctionCallbackInfo<v8::Value> &info);
        void AdoptNode(const v8::FunctionCallbackInfo<v8::Value> &info);
        void ImportNode(const v8::FunctionCallbackInfo<v8::Value> &info);

        void CreateAttribute(const v8::FunctionCallbackInfo<v8::Value> &info);
        void CreateAttributeNS(const v8::FunctionCallbackInfo<v8::Value> &info);
        void CreateCDATASection(const v8::FunctionCallbackInfo<v8::Value> &info);
        void CreateDocumentFragment(const v8::FunctionCallbackInfo<v8::Value> &info);
        void CreateElement(const v8::FunctionCallbackInfo<v8::Value> &info);
        void CreateElementNS(const v8::FunctionCallbackInfo<v8::Value> &info);
        void CreateTextNode(const v8::FunctionCallbackInfo<v8::Value> &info);
        void CreateComment(const v8::FunctionCallbackInfo<v8::Value> &info);

        void GetElementById(const v8::FunctionCallbackInfo<v8::Value> &info);
        void GetElementsByTagName(const v8::FunctionCallbackInfo<v8::Value> &info);
        void GetElementsByClassName(const v8::FunctionCallbackInfo<v8::Value> &info);
        void QuerySelector(const v8::FunctionCallbackInfo<v8::Value> &info);
        void QuerySelectorAll(const v8::FunctionCallbackInfo<v8::Value> &info);

        void Close(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Write(const v8::FunctionCallbackInfo<v8::Value> &info, bool newLine);
        void Write(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Writeln(const v8::FunctionCallbackInfo<v8::Value> &info);
      };
    }
  }
} // namespace endor