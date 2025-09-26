#include "./window.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  void Window::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    auto objectTemplate = tpl->InstanceTemplate();
    // TODO: Add properties and methods to the Window object here.
  }

  Local<ObjectTemplate> Window::GetInstanceTemplate(Isolate *isolate)
  {
    return GetFunctionTemplate(isolate)->InstanceTemplate();
  }

  Local<Object> Window::NewInstance(Isolate *isolate)
  {
    EscapableHandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    auto window = WindowBase::NewInstance(isolate, nullptr);
    window->Set(context,
                String::NewFromUtf8(isolate, "window").ToLocalChecked(),
                window)
      .Check();
    window->Set(context,
                String::NewFromUtf8(isolate, "self").ToLocalChecked(),
                window)
      .Check();
    return scope.Escape(window);
  }
}
