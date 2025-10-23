#include <client/script_bindings/canvas/text_metrics.hpp>

using namespace std;
using namespace v8;

namespace endor
{
  namespace script_bindings::canvas_bindings
  {
    // static
    void TextMetrics::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

      InstanceReadonlyPropertyAccessor(isolate, prototype, "width", &TextMetrics::WidthGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "actualBoundingBoxLeft", &TextMetrics::ActualBoundingBoxLeftGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "actualBoundingBoxRight", &TextMetrics::ActualBoundingBoxRightGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "fontBoundingBoxAscent", &TextMetrics::FontBoundingBoxAscentGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "fontBoundingBoxDescent", &TextMetrics::FontBoundingBoxDescentGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "actualBoundingBoxAscent", &TextMetrics::ActualBoundingBoxAscentGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "actualBoundingBoxDescent", &TextMetrics::ActualBoundingBoxDescentGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "emHeightAscent", &TextMetrics::EmHeightAscentGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "emHeightDescent", &TextMetrics::EmHeightDescentGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "hangingBaseline", &TextMetrics::HangingBaselineGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "alphabeticBaseline", &TextMetrics::AlphabeticBaselineGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "ideographicBaseline", &TextMetrics::IdeographicBaselineGetter);
    }

    // static
    Local<Object> TextMetrics::NewInstance(Isolate *isolate, const canvas::TextMetrics &metrics)
    {
      return TextMetricsBase::NewInstance(isolate, make_shared<canvas::TextMetrics>(metrics));
    }

    void TextMetrics::WidthGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      args.GetReturnValue().Set(Number::New(isolate, handle()->width));
    }

    void TextMetrics::ActualBoundingBoxLeftGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      args.GetReturnValue().Set(Number::New(isolate,
                                            handle()->actualBoundingBoxLeft));
    }

    void TextMetrics::ActualBoundingBoxRightGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      args.GetReturnValue().Set(Number::New(isolate,
                                            handle()->actualBoundingBoxRight));
    }

    void TextMetrics::FontBoundingBoxAscentGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      args.GetReturnValue().Set(Number::New(isolate,
                                            handle()->fontBoundingBoxAscent));
    }

    void TextMetrics::FontBoundingBoxDescentGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      args.GetReturnValue().Set(Number::New(isolate,
                                            handle()->fontBoundingBoxDescent));
    }

    void TextMetrics::ActualBoundingBoxAscentGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      args.GetReturnValue().Set(Number::New(isolate,
                                            handle()->actualBoundingBoxAscent));
    }

    void TextMetrics::ActualBoundingBoxDescentGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      args.GetReturnValue().Set(Number::New(isolate,
                                            handle()->actualBoundingBoxDescent));
    }

    void TextMetrics::EmHeightAscentGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      args.GetReturnValue().Set(Number::New(isolate,
                                            handle()->emHeightAscent));
    }

    void TextMetrics::EmHeightDescentGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      args.GetReturnValue().Set(Number::New(isolate,
                                            handle()->emHeightDescent));
    }

    void TextMetrics::HangingBaselineGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);
      args.GetReturnValue().Set(Number::New(isolate,
                                            handle()->hangingBaseline));
    }

    void TextMetrics::AlphabeticBaselineGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);
      args.GetReturnValue().Set(Number::New(isolate,
                                            handle()->alphabeticBaseline));
    }

    void TextMetrics::IdeographicBaselineGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);
      args.GetReturnValue().Set(Number::New(isolate,
                                            handle()->ideographicBaseline));
    }
  }
} // namespace endor
