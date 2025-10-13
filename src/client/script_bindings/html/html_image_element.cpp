#include <client/per_process.hpp>
#include <client/dom/browsing_context.hpp>
#include "./html_image_element.hpp"

namespace script_bindings::html_bindings
{
  using namespace std;
  using namespace v8;

  // static
  void HTMLImageElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);
    Local<ObjectTemplate> instance = tpl->PrototypeTemplate();

    InstanceReadonlyAccessor(isolate, instance, "currentSrc", &HTMLImageElement::CurrentSrcGetter);
    InstanceReadonlyAccessor(isolate, instance, "naturalWidth", &HTMLImageElement::NaturalWidthGetter);
    InstanceReadonlyAccessor(isolate, instance, "naturalHeight", &HTMLImageElement::NaturalHeightGetter);

    InstanceAccessor(isolate, instance, "src", &HTMLImageElement::SrcGetter, &HTMLImageElement::SrcSetter);
    InstanceAccessor(isolate, instance, "width", &HTMLImageElement::WidthGetter, &HTMLImageElement::WidthSetter);
    InstanceAccessor(isolate, instance, "height", &HTMLImageElement::HeightGetter, &HTMLImageElement::HeightSetter);
  }

  // static
  Local<Function> HTMLImageElement::CreateImageConstructor(Isolate *isolate)
  {
    return HTMLElement::CreateElementConstructor(isolate, "Image", HTMLImageElement::ImageConstructor);
  }

  // static
  void HTMLImageElement::ImageConstructor(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (!info.IsConstructCall())
    {
      MakeConstructorError(isolate,
                           "Please use the 'new' operator, this DOM object constructor cannot be called as a function.");
      return;
    }

    auto document = TrClientContextPerProcess::Get()->browsingContext->getActiveDocument();
    auto element = dynamic_pointer_cast<dom::HTMLImageElement>(document->createElement("img"));
    if (info.Length() >= 1 && info[0]->IsNumber())
    {
      int width = info[0]->Int32Value(context).ToChecked();
      element->setWidth(width);
    }
    if (info.Length() >= 2 && info[1]->IsNumber())
    {
      int height = info[1]->Int32Value(context).ToChecked();
      element->setHeight(height);
    }

    // Return the wrapped object.
    info.GetReturnValue().Set(NewInstance(isolate, element));
  }

  HTMLImageElement::HTMLImageElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLImageElementBase(isolate, args)
  {
  }

  void HTMLImageElement::CurrentSrcGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto currentSrc = handle()->currentSrc;
    info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                  currentSrc.c_str())
                                .ToLocalChecked());
  }

  void HTMLImageElement::NaturalWidthGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto naturalWidth = handle()->naturalWidth();
    info.GetReturnValue().Set(Integer::New(isolate, naturalWidth));
  }

  void HTMLImageElement::NaturalHeightGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto naturalHeight = handle()->naturalHeight();
    info.GetReturnValue().Set(Integer::New(isolate, naturalHeight));
  }

  void HTMLImageElement::SrcGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto src = handle()->getSrc();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                  src.c_str())
                                .ToLocalChecked());
  }

  void HTMLImageElement::SrcSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (!value->IsString())
    {
      handle()->setSrc("");
    }
    else
    {
      String::Utf8Value utf8Value(isolate, value);
      std::string newSrc(*utf8Value);

      handle()->setSrc(newSrc);
    }
  }

  void HTMLImageElement::WidthGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto width = handle()->width();
    info.GetReturnValue().Set(Integer::New(isolate, width));
  }

  void HTMLImageElement::WidthSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (value->IsNull())
    {
      handle()->setWidth(0);
    }
    else if (value->IsNumber())
    {
      int newWidth = value->Int32Value(context).ToChecked();
      handle()->setWidth(newWidth);
    }
    else
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgTypeError(isolate, "width", 0, "number", value)));
    }
  }

  void HTMLImageElement::HeightGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto height = handle()->height();
    info.GetReturnValue().Set(Integer::New(isolate, height));
  }

  void HTMLImageElement::HeightSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (value->IsNull())
    {
      handle()->setHeight(0);
    }
    else if (value->IsNumber())
    {
      int newHeight = value->Int32Value(context).ToChecked();
      handle()->setHeight(newHeight);
    }
    else
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgTypeError(isolate, "height", 0, "number", value)));
    }
  }
}
