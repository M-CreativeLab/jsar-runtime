#include "./html_canvas_element.hpp"
#include "../canvas/rendering_context2d.hpp"

namespace dombinding
{
  vector<Napi::ClassPropertyDescriptor<HTMLCanvasElement>> HTMLCanvasElement::GetClassProperties()
  {
    using T = HTMLCanvasElement;
    auto props = HTMLElementBase<HTMLCanvasElement, dom::HTMLCanvasElement>::GetClassProperties();
    auto added = vector<Napi::ClassPropertyDescriptor<HTMLCanvasElement>>(
        {
            T::InstanceAccessor("width", &T::WidthGetter, &T::WidthSetter),
            T::InstanceAccessor("height", &T::HeightGetter, &T::HeightSetter),
            T::InstanceMethod("getContext", &T::GetContext),
            T::InstanceMethod("toDataURL", &T::ToDataURL),
        });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  thread_local Napi::FunctionReference *HTMLCanvasElement::constructor;
  void HTMLCanvasElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "HTMLCanvasElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLCanvasElement", func);
  }

  HTMLCanvasElement::HTMLCanvasElement(const Napi::CallbackInfo &info)
      : HTMLElementBase(info),
        canvasbinding::CanvasWrap<canvasbinding::CanvasRenderingContext2D, canvas::Canvas>(node->canvasImpl())
  {
  }

  Napi::Value HTMLCanvasElement::WidthGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    auto self = info.This().As<Napi::Object>();
    return Napi::Number::New(env, node->getWidth());
  }

  void HTMLCanvasElement::WidthSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (value.IsNumber())
      node->setWidth(value.As<Napi::Number>().FloatValue());
  }

  Napi::Value HTMLCanvasElement::HeightGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    auto self = info.This().As<Napi::Object>();
    return Napi::Number::New(env, node->getHeight());
  }

  void HTMLCanvasElement::HeightSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (value.IsNumber())
      node->setHeight(value.As<Napi::Number>().FloatValue());
  }

  Napi::Value HTMLCanvasElement::ToDataURL(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    string type = "image/png";
    double encoderOptions = 0.92;

    if (info.Length() >= 1)
      type = info[0].ToString().Utf8Value();
    if (info.Length() >= 2)
      encoderOptions = info[1].ToNumber().DoubleValue();
    return Napi::String::New(env, node->toDataURL(type, encoderOptions));
  }
}
