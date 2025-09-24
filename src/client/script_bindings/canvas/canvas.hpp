#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/canvas/canvas.hpp>
#include <client/dom/dom_event_target.hpp>

namespace script_bindings
{
  namespace canvas_bindings
  {
    class Canvas;
    using CanvasBase = scripting_base::ObjectWrap<Canvas, ::canvas::OffscreenCanvas>;

    /**
     * Canvas wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps canvas::OffscreenCanvas objects for use in V8 JavaScript execution contexts.
     * It provides the standard Canvas interface including getContext, toDataURL, and toBlob methods.
     */
    class Canvas : public CanvasBase
    {
      using CanvasBase::ObjectWrap;

    public:
      /**
       * The name of the Canvas class for V8.
       */
      static std::string Name()
      {
        return "Canvas";
      }

      /**
       * Configure the V8 function template with Canvas properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 Canvas instance from a native canvas::OffscreenCanvas.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::canvas::OffscreenCanvas> nativeCanvas);

      /**
       * Initialize the Canvas class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      Canvas(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void WidthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void HeightGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

      // Property setters
      static void WidthSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void HeightSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Methods
      static void GetContext(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void ToDataURL(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void ToBlob(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void TransferToImageBitmap(const v8::FunctionCallbackInfo<v8::Value> &info);
    };

    class OffscreenCanvas;
    using OffscreenCanvasBase = scripting_base::ObjectWrap<OffscreenCanvas, ::canvas::OffscreenCanvas>;

    /**
     * OffscreenCanvas wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps canvas::OffscreenCanvas objects for use in V8 JavaScript execution contexts.
     * It provides the standard OffscreenCanvas interface for off-screen rendering.
     */
    class OffscreenCanvas : public OffscreenCanvasBase
    {
      using OffscreenCanvasBase::ObjectWrap;

    public:
      /**
       * The name of the OffscreenCanvas class for V8.
       */
      static std::string Name()
      {
        return "OffscreenCanvas";
      }

      /**
       * Configure the V8 function template with OffscreenCanvas properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 OffscreenCanvas instance from a native canvas::OffscreenCanvas.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::canvas::OffscreenCanvas> nativeCanvas);

      /**
       * Initialize the OffscreenCanvas class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      OffscreenCanvas(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void WidthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void HeightGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

      // Property setters
      static void WidthSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void HeightSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Methods
      static void GetContext(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void ConvertToBlob(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void TransferToImageBitmap(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}
