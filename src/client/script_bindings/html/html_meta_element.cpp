#include "./html_meta_element.hpp"

namespace endor
{
  namespace script_bindings::html_bindings
  {
    using namespace std;
    using namespace v8;

    // static
    void HTMLMetaElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instance = tpl->InstanceTemplate();

      // Accessors
      InstanceAccessor(isolate, instance, "content", &HTMLMetaElement::ContentGetter, &HTMLMetaElement::ContentSetter);
      InstanceAccessor(isolate, instance, "name", &HTMLMetaElement::NameGetter, &HTMLMetaElement::NameSetter);
      InstanceAccessor(isolate, instance, "httpEquiv", &HTMLMetaElement::HttpEquivGetter, &HTMLMetaElement::HttpEquivSetter);
      InstanceAccessor(isolate, instance, "media", &HTMLMetaElement::MediaGetter, &HTMLMetaElement::MediaSetter);
    }

    void HTMLMetaElement::ContentGetter(const PropertyCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      auto content = handle()->getContent();
      args.GetReturnValue().Set(String::NewFromUtf8(isolate, content.c_str()).ToLocalChecked());
    }

    void HTMLMetaElement::ContentSetter(Local<Value> value, const PropertyCallbackInfo<void> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      if (!value->IsString())
      {
        handle()->setContent("");
      }
      else
      {
        String::Utf8Value utf8Value(isolate, value);
        string content(*utf8Value);

        handle()->setContent(content);
      }
    }

    void HTMLMetaElement::NameGetter(const PropertyCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      auto name = handle()->getName();
      args.GetReturnValue().Set(String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked());
    }

    void HTMLMetaElement::NameSetter(Local<Value> value, const PropertyCallbackInfo<void> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      if (!value->IsString())
      {
        handle()->setName("");
      }
      else
      {
        String::Utf8Value utf8Value(isolate, value);
        string name(*utf8Value);

        handle()->setName(name);
      }
    }

    void HTMLMetaElement::HttpEquivGetter(const PropertyCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      auto httpEquiv = handle()->getHttpEquiv();
      args.GetReturnValue().Set(String::NewFromUtf8(isolate, httpEquiv.c_str()).ToLocalChecked());
    }

    void HTMLMetaElement::HttpEquivSetter(Local<Value> value, const PropertyCallbackInfo<void> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      if (!value->IsString())
      {
        handle()->setHttpEquiv("");
      }
      else
      {
        String::Utf8Value utf8Value(isolate, value);
        string httpEquiv(*utf8Value);

        handle()->setHttpEquiv(httpEquiv);
      }
    }

    void HTMLMetaElement::MediaGetter(const PropertyCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      auto media = handle()->getMedia();
      args.GetReturnValue().Set(String::NewFromUtf8(isolate, media.c_str()).ToLocalChecked());
    }

    void HTMLMetaElement::MediaSetter(Local<Value> value, const PropertyCallbackInfo<void> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      if (!value->IsString())
      {
        handle()->setMedia("");
      }
      else
      {
        String::Utf8Value utf8Value(isolate, value);
        string media(*utf8Value);

        handle()->setMedia(media);
      }
    }
  }
} // namespace endor
