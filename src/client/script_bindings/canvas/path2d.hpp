#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/canvas/path2d.hpp>

namespace script_bindings
{
  namespace canvas_bindings
  {
    class Path2D;
    using Path2DBase = scripting_base::ObjectWrap<Path2D, ::canvas::Path2D>;

    /**
     * Path2D wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps canvas::Path2D objects for use in V8 JavaScript execution contexts.
     * It provides the standard Path2D interface for creating and manipulating 2D paths.
     */
    class Path2D : public Path2DBase
    {
      using Path2DBase::ObjectWrap;

    public:
      /**
       * The name of the Path2D class for V8.
       */
      static std::string Name()
      {
        return "Path2D";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::canvas::Path2D> nativePath2D);
    public:
      Path2D(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Path building methods
      static void AddPath(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void ClosePath(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void MoveTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void LineTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void BezierCurveTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void QuadraticCurveTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Arc(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void ArcTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Ellipse(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Rect(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void RoundRect(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}