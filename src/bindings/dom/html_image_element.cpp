#include <assert.h>
#include "./html_image_element.hpp"
#include "./document-inl.hpp"

using namespace std;

namespace dombinding
{
  vector<Napi::ClassPropertyDescriptor<HTMLImageElement>> HTMLImageElement::GetClassProperties(Napi::Env env)
  {
    using T = HTMLImageElement;
    auto props = HTMLElementBase<HTMLImageElement, dom::HTMLImageElement>::GetClassProperties(env);
    auto added = vector<Napi::ClassPropertyDescriptor<HTMLImageElement>>(
        {
            T::InstanceAccessor("currentSrc", &T::CurrentSrcGetter, nullptr),
            T::InstanceAccessor("src", &T::SrcGetter, &T::SrcSetter),
            T::InstanceAccessor("naturalWidth", &T::NaturalWidthGetter, nullptr),
            T::InstanceAccessor("naturalHeight", &T::NaturalHeightGetter, nullptr),
            T::InstanceAccessor("width", &T::WidthGetter, &T::WidthSetter),
            T::InstanceAccessor("height", &T::HeightGetter, &T::HeightSetter),
        });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  thread_local Napi::FunctionReference *HTMLImageElement::constructor;
  void HTMLImageElement::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, "HTMLImageElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    auto global = env.Global();
    global.Set("HTMLImageElement", func);
    global.Set("Image", Napi::Function::New(env, HTMLImageElement::ImageConstructor));
  }

  Napi::Value HTMLImageElement::ImageConstructor(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (!info.IsConstructCall())
    {
      auto msg = "Failed to construct 'Image': "
                 "Please use the 'new' operator, this object constructor cannot be called as a function.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Null();
    }

    Document *document = Document::GetCurrent(env);
    if (TR_UNLIKELY(document == nullptr))
    {
      auto msg = "Failed to construct 'Image': "
                 "The global object 'document' is not an instance of 'Document'.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Null();
    }

    auto documentObject = document->Value();
    auto imageValue = documentObject
        .Get("createElement")
        .As<Napi::Function>()
        .Call(documentObject, {Napi::String::New(env, "img")});

    auto imageObject = imageValue.As<Napi::Object>();
    if (info.Length() >= 1 && info[0].IsNumber())
    {
      auto width = info[0].As<Napi::Number>().Int32Value();
      imageObject.Set("width", Napi::Number::New(env, width));
    }
    if (info.Length() >= 2 && info[1].IsNumber())
    {
      auto height = info[1].As<Napi::Number>().Int32Value();
      imageObject.Set("height", Napi::Number::New(env, height));
    }
    return imageValue;
  }

  HTMLImageElement::HTMLImageElement(const Napi::CallbackInfo &info)
      : HTMLElementBase(info),
        canvasbinding::ImageSourceWrap<dom::HTMLImageElement>()
  {
    dataImpl = node;
  }

  Napi::Value HTMLImageElement::CurrentSrcGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(info.Env(), node->currentSrc);
  }

  Napi::Value HTMLImageElement::SrcGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(info.Env(), node->getSrc());
  }

  void HTMLImageElement::SrcSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    string newSrc = value.ToString().Utf8Value();
    node->setSrc(newSrc);
  }

  Napi::Value HTMLImageElement::NaturalWidthGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(info.Env(), node->width());
  }

  Napi::Value HTMLImageElement::NaturalHeightGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(info.Env(), node->height());
  }

  Napi::Value HTMLImageElement::WidthGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(info.Env(), node->width());
  }

  void HTMLImageElement::WidthSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (value.IsNumber())
    {
      int newWidth = value.As<Napi::Number>().Int32Value();
      node->setWidth(newWidth);
    }
    else if (value.IsNull())
    {
      node->setWidth(0);
    }
    else
    {
      Napi::TypeError::New(env, "The value must be a number or null.").ThrowAsJavaScriptException();
    }
  }

  Napi::Value HTMLImageElement::HeightGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(info.Env(), node->height());
  }

  void HTMLImageElement::HeightSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (value.IsNumber())
    {
      int newHeight = value.As<Napi::Number>().Int32Value();
      node->setHeight(newHeight);
    }
    else if (value.IsNull())
    {
      node->setHeight(0);
    }
    else
    {
      Napi::TypeError::New(env, "The value must be a number or null.").ThrowAsJavaScriptException();
    }
  }
}
