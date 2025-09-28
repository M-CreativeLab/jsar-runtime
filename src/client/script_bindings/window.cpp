#include <memory>
#include "./window.hpp"
#include "./location.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  void Window::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope handle_scope(isolate);
    auto context = isolate->GetCurrentContext();
    auto prototype_template = tpl->PrototypeTemplate();
    auto instance_template = tpl->InstanceTemplate();

    InstanceAccessor(isolate,
                     instance_template,
                     "location",
                     &Window::LocationGetter,
                     &Window::LocationSetter);

    instance_template->Set(String::NewFromUtf8(isolate, "alert").ToLocalChecked(),
                           FunctionTemplate::New(isolate, Alert));
    instance_template->Set(String::NewFromUtf8(isolate, "blur").ToLocalChecked(),
                           FunctionTemplate::New(isolate, Blur));
    instance_template->Set(String::NewFromUtf8(isolate, "close").ToLocalChecked(),
                           FunctionTemplate::New(isolate, Close));
    instance_template->Set(String::NewFromUtf8(isolate, "confirm").ToLocalChecked(),
                           FunctionTemplate::New(isolate, Confirm));
    instance_template->Set(String::NewFromUtf8(isolate, "focus").ToLocalChecked(),
                           FunctionTemplate::New(isolate, Focus));
    instance_template->Set(String::NewFromUtf8(isolate, "open").ToLocalChecked(),
                           FunctionTemplate::New(isolate, Open));
    instance_template->Set(String::NewFromUtf8(isolate, "prompt").ToLocalChecked(),
                           FunctionTemplate::New(isolate, Prompt));
  }

  Local<ObjectTemplate> Window::GetInstanceTemplate(Isolate *isolate)
  {
    return GetFunctionTemplate(isolate)->InstanceTemplate();
  }

  void Window::LocationGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Local<Object> location = Location::GetOrNewInstance(isolate, handle()->location());
    info.GetReturnValue().Set(location);
  }

  void Window::LocationSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    isolate->ThrowException(v8::Exception::TypeError(
      MakeMethodError(isolate, "location", "Cannot set property location of Window")));
    return;
  }

  void Window::Alert(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    info.GetReturnValue().SetNull();
  }

  void Window::Blur(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    info.GetReturnValue().SetNull();
  }

  void Window::Close(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    info.GetReturnValue().SetNull();
  }

  void Window::Confirm(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    info.GetReturnValue().SetNull();
  }

  void Window::Focus(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    info.GetReturnValue().SetNull();
  }

  void Window::Open(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    info.GetReturnValue().SetNull();
  }

  void Window::Prompt(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    info.GetReturnValue().SetNull();
  }
}
