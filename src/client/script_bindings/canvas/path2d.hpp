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
      static std::string Name()
      {
        return "Path2D";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      Path2D(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Path building methods
      void AddPath(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ClosePath(const v8::FunctionCallbackInfo<v8::Value> &info);
      void MoveTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      void LineTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      void BezierCurveTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      void QuadraticCurveTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Arc(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ArcTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Ellipse(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Rect(const v8::FunctionCallbackInfo<v8::Value> &info);
      void RoundRect(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}