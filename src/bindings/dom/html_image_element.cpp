#include <assert.h>
#include "./html_image_element.hpp"

namespace dombinding
{
  Napi::FunctionReference *HTMLImageElement::constructor;

  vector<Napi::ClassPropertyDescriptor<HTMLImageElement>> HTMLImageElement::GetClassProperties()
  {
    using T = HTMLImageElement;
    auto props = HTMLElementBase<HTMLImageElement, dom::HTMLImageElement>::GetClassProperties();
    auto added = vector<Napi::ClassPropertyDescriptor<HTMLImageElement>>(
        {
            T::InstanceAccessor("currentSrc", &T::CurrentSrcGetter, nullptr),
        });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  void HTMLImageElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "HTMLImageElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLImageElement", func);
  }

  Napi::Value HTMLImageElement::CurrentSrcGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(info.Env(), this->node->currentSrc);
  }
}
