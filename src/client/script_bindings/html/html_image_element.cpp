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
}
