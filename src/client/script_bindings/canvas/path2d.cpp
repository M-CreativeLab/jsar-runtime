#include "path2d.hpp"
#include <client/canvas/path2d.hpp>

namespace script_bindings
{
  namespace canvas
  {
    using namespace v8;

    void Path2D::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      tpl->SetClassName(String::NewFromUtf8(isolate, "Path2D").ToLocalChecked());

      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();
      instanceTemplate->SetInternalFieldCount(1);

      // Path building methods
      instanceTemplate->Set(String::NewFromUtf8(isolate, "addPath").ToLocalChecked(),
                            FunctionTemplate::New(isolate, AddPath));
      instanceTemplate->Set(String::NewFromUtf8(isolate, "closePath").ToLocalChecked(),
                            FunctionTemplate::New(isolate, ClosePath));
      instanceTemplate->Set(String::NewFromUtf8(isolate, "moveTo").ToLocalChecked(),
                            FunctionTemplate::New(isolate, MoveTo));
      instanceTemplate->Set(String::NewFromUtf8(isolate, "lineTo").ToLocalChecked(),
                            FunctionTemplate::New(isolate, LineTo));
      instanceTemplate->Set(String::NewFromUtf8(isolate, "bezierCurveTo").ToLocalChecked(),
                            FunctionTemplate::New(isolate, BezierCurveTo));
      instanceTemplate->Set(String::NewFromUtf8(isolate, "quadraticCurveTo").ToLocalChecked(),
                            FunctionTemplate::New(isolate, QuadraticCurveTo));
      instanceTemplate->Set(String::NewFromUtf8(isolate, "arc").ToLocalChecked(),
                            FunctionTemplate::New(isolate, Arc));
      instanceTemplate->Set(String::NewFromUtf8(isolate, "arcTo").ToLocalChecked(),
                            FunctionTemplate::New(isolate, ArcTo));
      instanceTemplate->Set(String::NewFromUtf8(isolate, "ellipse").ToLocalChecked(),
                            FunctionTemplate::New(isolate, Ellipse));
      instanceTemplate->Set(String::NewFromUtf8(isolate, "rect").ToLocalChecked(),
                            FunctionTemplate::New(isolate, Rect));
      instanceTemplate->Set(String::NewFromUtf8(isolate, "roundRect").ToLocalChecked(),
                            FunctionTemplate::New(isolate, RoundRect));
    }

    Local<Object> Path2D::NewInstance(Isolate *isolate, std::shared_ptr<::canvas::Path2D> nativePath2D)
    {
      Local<Function> constructor = GetConstructorFunction(isolate);
      Local<Context> context = isolate->GetCurrentContext();
      Local<Object> instance = constructor->NewInstance(context).ToLocalChecked();

      Path2D *path2DWrapper = ObjectWrap::Unwrap<Path2D>(instance);
      path2DWrapper->SetNativeInstance(nativePath2D);

      return instance;
    }

    Local<Function> Path2D::Initialize(Isolate *isolate)
    {
      return ObjectWrap::Initialize(isolate);
    }

    Path2D::Path2D(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : Path2DBase(isolate, args)
    {
      // Constructor implementation
      auto nativePath2D = std::make_shared<::canvas::Path2D>();

      if (args.Length() >= 1)
      {
        if (args[0]->IsString())
        {
          // Constructor with SVG path string
          String::Utf8Value pathString(isolate, args[0]);
          nativePath2D->addPath(*pathString);
        }
        else if (args[0]->IsObject())
        {
          // Constructor with another Path2D object
          // TODO: Copy path from another Path2D
        }
      }

      SetNativeInstance(nativePath2D);
    }

    void Path2D::AddPath(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Path2D *path2D = ObjectWrap::Unwrap<Path2D>(info.Holder());

      if (!path2D || !path2D->GetNativeInstance())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid Path2D instance").ToLocalChecked()));
        return;
      }

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "addPath requires at least 1 argument").ToLocalChecked()));
        return;
      }

      // TODO: Implement addPath method
      info.GetReturnValue().SetUndefined();
    }

    void Path2D::ClosePath(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Path2D *path2D = ObjectWrap::Unwrap<Path2D>(info.Holder());

      if (!path2D || !path2D->GetNativeInstance())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid Path2D instance").ToLocalChecked()));
        return;
      }

      path2D->GetNativeInstance()->closePath();
      info.GetReturnValue().SetUndefined();
    }

    void Path2D::MoveTo(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Path2D *path2D = ObjectWrap::Unwrap<Path2D>(info.Holder());

      if (!path2D || !path2D->GetNativeInstance())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid Path2D instance").ToLocalChecked()));
        return;
      }

      if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "moveTo requires 2 numeric arguments").ToLocalChecked()));
        return;
      }

      double x = info[0]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double y = info[1]->NumberValue(isolate->GetCurrentContext()).FromJust();

      path2D->GetNativeInstance()->moveTo(x, y);
      info.GetReturnValue().SetUndefined();
    }

    void Path2D::LineTo(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Path2D *path2D = ObjectWrap::Unwrap<Path2D>(info.Holder());

      if (!path2D || !path2D->GetNativeInstance())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid Path2D instance").ToLocalChecked()));
        return;
      }

      if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "lineTo requires 2 numeric arguments").ToLocalChecked()));
        return;
      }

      double x = info[0]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double y = info[1]->NumberValue(isolate->GetCurrentContext()).FromJust();

      path2D->GetNativeInstance()->lineTo(x, y);
      info.GetReturnValue().SetUndefined();
    }

    void Path2D::BezierCurveTo(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Path2D *path2D = ObjectWrap::Unwrap<Path2D>(info.Holder());

      if (!path2D || !path2D->GetNativeInstance())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid Path2D instance").ToLocalChecked()));
        return;
      }

      if (info.Length() < 6)
      {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "bezierCurveTo requires 6 numeric arguments").ToLocalChecked()));
        return;
      }

      double cp1x = info[0]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double cp1y = info[1]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double cp2x = info[2]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double cp2y = info[3]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double x = info[4]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double y = info[5]->NumberValue(isolate->GetCurrentContext()).FromJust();

      path2D->GetNativeInstance()->bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y);
      info.GetReturnValue().SetUndefined();
    }

    void Path2D::QuadraticCurveTo(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Path2D *path2D = ObjectWrap::Unwrap<Path2D>(info.Holder());

      if (!path2D || !path2D->GetNativeInstance())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid Path2D instance").ToLocalChecked()));
        return;
      }

      if (info.Length() < 4)
      {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "quadraticCurveTo requires 4 numeric arguments").ToLocalChecked()));
        return;
      }

      double cpx = info[0]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double cpy = info[1]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double x = info[2]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double y = info[3]->NumberValue(isolate->GetCurrentContext()).FromJust();

      path2D->GetNativeInstance()->quadraticCurveTo(cpx, cpy, x, y);
      info.GetReturnValue().SetUndefined();
    }

    void Path2D::Arc(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Path2D *path2D = ObjectWrap::Unwrap<Path2D>(info.Holder());

      if (!path2D || !path2D->GetNativeInstance())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid Path2D instance").ToLocalChecked()));
        return;
      }

      if (info.Length() < 5)
      {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "arc requires at least 5 arguments").ToLocalChecked()));
        return;
      }

      double x = info[0]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double y = info[1]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double radius = info[2]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double startAngle = info[3]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double endAngle = info[4]->NumberValue(isolate->GetCurrentContext()).FromJust();
      bool counterclockwise = info.Length() > 5 ? info[5]->BooleanValue(isolate) : false;

      path2D->GetNativeInstance()->arc(x, y, radius, startAngle, endAngle, counterclockwise);
      info.GetReturnValue().SetUndefined();
    }

    void Path2D::ArcTo(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Path2D *path2D = ObjectWrap::Unwrap<Path2D>(info.Holder());

      if (!path2D || !path2D->GetNativeInstance())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid Path2D instance").ToLocalChecked()));
        return;
      }

      if (info.Length() < 5)
      {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "arcTo requires 5 numeric arguments").ToLocalChecked()));
        return;
      }

      double x1 = info[0]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double y1 = info[1]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double x2 = info[2]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double y2 = info[3]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double radius = info[4]->NumberValue(isolate->GetCurrentContext()).FromJust();

      path2D->GetNativeInstance()->arcTo(x1, y1, x2, y2, radius);
      info.GetReturnValue().SetUndefined();
    }

    void Path2D::Ellipse(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Path2D *path2D = ObjectWrap::Unwrap<Path2D>(info.Holder());

      if (!path2D || !path2D->GetNativeInstance())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid Path2D instance").ToLocalChecked()));
        return;
      }

      if (info.Length() < 7)
      {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "ellipse requires at least 7 arguments").ToLocalChecked()));
        return;
      }

      double x = info[0]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double y = info[1]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double radiusX = info[2]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double radiusY = info[3]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double rotation = info[4]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double startAngle = info[5]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double endAngle = info[6]->NumberValue(isolate->GetCurrentContext()).FromJust();
      bool counterclockwise = info.Length() > 7 ? info[7]->BooleanValue(isolate) : false;

      path2D->GetNativeInstance()->ellipse(x, y, radiusX, radiusY, rotation, startAngle, endAngle, counterclockwise);
      info.GetReturnValue().SetUndefined();
    }

    void Path2D::Rect(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Path2D *path2D = ObjectWrap::Unwrap<Path2D>(info.Holder());

      if (!path2D || !path2D->GetNativeInstance())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid Path2D instance").ToLocalChecked()));
        return;
      }

      if (info.Length() < 4)
      {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "rect requires 4 numeric arguments").ToLocalChecked()));
        return;
      }

      double x = info[0]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double y = info[1]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double width = info[2]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double height = info[3]->NumberValue(isolate->GetCurrentContext()).FromJust();

      path2D->GetNativeInstance()->rect(x, y, width, height);
      info.GetReturnValue().SetUndefined();
    }

    void Path2D::RoundRect(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Path2D *path2D = ObjectWrap::Unwrap<Path2D>(info.Holder());

      if (!path2D || !path2D->GetNativeInstance())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid Path2D instance").ToLocalChecked()));
        return;
      }

      if (info.Length() < 5)
      {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "roundRect requires at least 5 arguments").ToLocalChecked()));
        return;
      }

      double x = info[0]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double y = info[1]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double width = info[2]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double height = info[3]->NumberValue(isolate->GetCurrentContext()).FromJust();
      double radius = info[4]->NumberValue(isolate->GetCurrentContext()).FromJust();

      path2D->GetNativeInstance()->roundRect(x, y, width, height, radius);
      info.GetReturnValue().SetUndefined();
    }
  }
}