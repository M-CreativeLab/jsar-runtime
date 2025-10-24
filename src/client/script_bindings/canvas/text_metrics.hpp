#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/canvas/text_metrics.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace canvas_bindings
    {
      class TextMetrics;
      using TextMetricsBase = scripting_base::ObjectWrap<TextMetrics, canvas::TextMetrics>;

      class TextMetrics : public TextMetricsBase
      {
        using TextMetricsBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "TextMetrics";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
        static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, const canvas::TextMetrics &metrics);

      private:
        // Path building methods
        void WidthGetter(const v8::FunctionCallbackInfo<v8::Value> &);
        void ActualBoundingBoxLeftGetter(const v8::FunctionCallbackInfo<v8::Value> &);
        void ActualBoundingBoxRightGetter(const v8::FunctionCallbackInfo<v8::Value> &);
        void FontBoundingBoxAscentGetter(const v8::FunctionCallbackInfo<v8::Value> &);
        void FontBoundingBoxDescentGetter(const v8::FunctionCallbackInfo<v8::Value> &);
        void ActualBoundingBoxAscentGetter(const v8::FunctionCallbackInfo<v8::Value> &);
        void ActualBoundingBoxDescentGetter(const v8::FunctionCallbackInfo<v8::Value> &);
        void EmHeightAscentGetter(const v8::FunctionCallbackInfo<v8::Value> &);
        void EmHeightDescentGetter(const v8::FunctionCallbackInfo<v8::Value> &);
        void HangingBaselineGetter(const v8::FunctionCallbackInfo<v8::Value> &);
        void AlphabeticBaselineGetter(const v8::FunctionCallbackInfo<v8::Value> &);
        void IdeographicBaselineGetter(const v8::FunctionCallbackInfo<v8::Value> &);
      };
    }
  }
} // namespace endor
