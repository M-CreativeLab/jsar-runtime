#include <iostream>
#include <client/script_bindings/dom/document.hpp>
#include <client/script_bindings/html/html_element.hpp>
#include "./element.hpp"

using namespace std;
using namespace v8;

namespace endor
{
  namespace script_bindings
  {
    namespace dom_bindings
    {
      void Element::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
      {
        HandleScope scope(isolate);

        // Set up the instance template
        Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

        // Add property accessors
        InstanceReadonlyPropertyAccessor(isolate, prototype, "tagName", &Element::TagNameGetter);
        InstanceReadonlyPropertyAccessor(isolate, prototype, "classList", &Element::ClassListGetter);
        InstanceReadonlyPropertyAccessor(isolate, prototype, "children", &Element::ChildrenGetter);
        InstanceReadonlyPropertyAccessor(isolate, prototype, "firstElementChild", &Element::FirstElementChildGetter);
        InstanceReadonlyPropertyAccessor(isolate, prototype, "lastElementChild", &Element::LastElementChildGetter);
        InstancePropertyAccessor(isolate, prototype, "id", &Element::IdGetter, &Element::IdSetter);
        InstancePropertyAccessor(isolate,
                                 prototype,
                                 "className",
                                 &Element::ClassNameGetter,
                                 &Element::ClassNameSetter);
        InstancePropertyAccessor(isolate,
                                 prototype,
                                 "innerHTML",
                                 &Element::InnerHTMLGetter,
                                 &Element::InnerHTMLSetter,
                                 PropertyAttribute::DontEnum);
        InstancePropertyAccessor(isolate,
                                 prototype,
                                 "outerHTML",
                                 &Element::OuterHTMLGetter,
                                 &Element::OuterHTMLSetter,
                                 PropertyAttribute::DontEnum);

        // Add methods
        InstanceMethod(isolate, prototype, "after", &Element::After);
        InstanceMethod(isolate, prototype, "animate", &Element::Animate);
        InstanceMethod(isolate, prototype, "append", &Element::Append);
        InstanceMethod(isolate, prototype, "attachShadow", &Element::AttachShadow);
        InstanceMethod(isolate, prototype, "before", &Element::Before);
        InstanceMethod(isolate, prototype, "checkVisibility", &Element::CheckVisibility);
        InstanceMethod(isolate, prototype, "closest", &Element::Closest);
        InstanceMethod(isolate, prototype, "getAttribute", &Element::GetAttribute);
        InstanceMethod(isolate, prototype, "getAttributeNames", &Element::GetAttributeNames);
        InstanceMethod(isolate, prototype, "getBoundingClientRect", &Element::GetBoundingClientRect);
        InstanceMethod(isolate, prototype, "hasAttribute", &Element::HasAttribute);
        InstanceMethod(isolate, prototype, "hasAttributes", &Element::HasAttributes);
        InstanceMethod(isolate, prototype, "remove", &Element::Remove);
        InstanceMethod(isolate, prototype, "removeAttribute", &Element::RemoveAttribute);
        InstanceMethod(isolate, prototype, "setAttribute", &Element::SetAttribute);
        InstanceMethod(isolate, prototype, "setAttributeNode", &Element::SetAttributeNode);
        InstanceMethod(isolate, prototype, "setAttributeNodeNS", &Element::SetAttributeNodeNS);
        InstanceMethod(isolate, prototype, "setAttributeNS", &Element::SetAttributeNS);
        InstanceMethod(isolate, prototype, "getElementsByTagName", &Element::GetElementsByTagName);
        InstanceMethod(isolate, prototype, "getElementsByClassName", &Element::GetElementsByClassName);
        InstanceMethod(isolate, prototype, "querySelector", &Element::QuerySelector);
        InstanceMethod(isolate, prototype, "querySelectorAll", &Element::QuerySelectorAll);
      }

      Local<Object> Element::NewInstance(Isolate *isolate, std::shared_ptr<::endor::dom::Element> nativeElement)
      {
        EscapableHandleScope scope(isolate);
        assert(nativeElement != nullptr && "The native element is null.");

        if (nativeElement->isHTMLElement())
        {
          auto htmlElement = static_pointer_cast<::endor::dom::HTMLElement>(nativeElement);
          return scope.Escape(html_bindings::HTMLElement::NewInstance(isolate, htmlElement));
        }
        else
        {
          return scope.Escape(ElementBase::NewInstance(isolate, nativeElement).As<Object>());
        }
      }

      Element::Element(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
          : ElementBase(isolate, args)
      {
      }

      // Property getters and setters

      void Element::TagNameGetter(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        string tagName = handle()->tagName;
        info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                      tagName.c_str())
                                    .ToLocalChecked());
      }

      void Element::IdGetter(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                      handle()->id.c_str())
                                    .ToLocalChecked());
      }

      void Element::IdSetter(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        {
          String::Utf8Value utf8Value(isolate, info[0]);
          handle()->setId(string(*utf8Value));
        }
      }

      void Element::ClassListGetter(const v8::FunctionCallbackInfo<v8::Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "classList", "Not implemented")));
      }

      void Element::ClassNameGetter(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                      handle()->className().c_str())
                                    .ToLocalChecked());
      }

      void Element::ClassNameSetter(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        {
          String::Utf8Value utf8Value(isolate, info[0]);
          handle()->setClassName(string(*utf8Value));
        }
      }

      void Element::ChildrenGetter(const v8::FunctionCallbackInfo<v8::Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "children", "Not implemented")));
      }

      void Element::FirstElementChildGetter(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        auto element = handle()->firstElementChild();
        if (element != nullptr)
        {
          auto elementWrapper = Element::NewInstance(isolate, element);
          info.GetReturnValue().Set(elementWrapper);
        }
        else
        {
          info.GetReturnValue().SetNull();
        }
      }

      void Element::LastElementChildGetter(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        auto element = handle()->lastElementChild();
        if (element != nullptr)
        {
          auto elementWrapper = Element::NewInstance(isolate, element);
          info.GetReturnValue().Set(elementWrapper);
        }
        else
        {
          info.GetReturnValue().SetNull();
        }
      }

      void Element::InnerHTMLGetter(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                      handle()->getInnerHTML().c_str())
                                    .ToLocalChecked());
      }

      void Element::InnerHTMLSetter(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        {
          String::Utf8Value utf8Value(isolate, info[0]);
          handle()->setInnerHTML(string(*utf8Value));
        }
      }

      void Element::OuterHTMLGetter(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                      handle()->getOuterHTML().c_str())
                                    .ToLocalChecked());
      }

      void Element::OuterHTMLSetter(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        {
          String::Utf8Value utf8Value(isolate, info[0]);
          handle()->setOuterHTML(string(*utf8Value));
        }
      }

      // Methods

      void Element::After(const v8::FunctionCallbackInfo<v8::Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "after", "1 or more arguments required")));
          return;
        }

        if (info.Length() == 1)
        {
          if (Node::IsInstanceOf(isolate, info[0]))
          {
            auto nodeBinding = Node::Unwrap(isolate, info[0].As<Object>());
            if (nodeBinding != nullptr && nodeBinding->hasData())
            {
              handle()->after(nodeBinding->handle());
            }
            info.GetReturnValue().SetUndefined();
            return;
          }
          else if (info[0]->IsString())
          {
            String::Utf8Value utf8Value(isolate, info[0]);
            handle()->after(string(*utf8Value));
            info.GetReturnValue().SetUndefined();
            return;
          }
          else
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodError(isolate, "after", "Argument must be a Node or string")));
            return;
          }
        }
        else
        {
          vector<shared_ptr<dom::Node>> nodes;
          for (int i = 0; i < info.Length(); ++i)
          {
            if (Node::IsInstanceOf(isolate, info[i]))
            {
              auto nodeBinding = Node::Unwrap(isolate, info[i].As<Object>());
              if (nodeBinding != nullptr && nodeBinding->hasData())
              {
                nodes.push_back(nodeBinding->handle());
              }
            }
            else if (info[i]->IsString())
            {
              String::Utf8Value utf8Value(isolate, info[i]);
              shared_ptr<dom::Text> textNode = handle()->getOwnerDocumentChecked().createTextNode(string(*utf8Value));
              nodes.push_back(textNode);
            }
            else
            {
              isolate->ThrowException(Exception::TypeError(
                MakeMethodError(isolate, "after", "All arguments must be Node or string")));
              return;
            }
          }
          handle()->after(nodes);
          info.GetReturnValue().SetUndefined();
        }
      }

      void Element::Animate(const v8::FunctionCallbackInfo<v8::Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "animate", "Not implemented")));
      }

      void Element::Append(const v8::FunctionCallbackInfo<v8::Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "append", "Not implemented")));
      }

      void Element::AttachShadow(const v8::FunctionCallbackInfo<v8::Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "attachShadow", "Not implemented")));
      }

      void Element::Before(const v8::FunctionCallbackInfo<v8::Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "before", "1 or more arguments required")));
          return;
        }

        if (info.Length() == 1)
        {
          if (Node::IsInstanceOf(isolate, info[0]))
          {
            auto nodeBinding = Node::Unwrap(isolate, info[0].As<Object>());
            if (nodeBinding != nullptr && nodeBinding->hasData())
            {
              handle()->before(nodeBinding->handle());
            }
            info.GetReturnValue().SetUndefined();
            return;
          }
          else if (info[0]->IsString())
          {
            String::Utf8Value utf8Value(isolate, info[0]);
            handle()->before(string(*utf8Value));
            info.GetReturnValue().SetUndefined();
            return;
          }
          else
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodError(isolate, "after", "Argument must be a Node or string")));
            return;
          }
        }
        else
        {
          vector<shared_ptr<dom::Node>> nodes;
          for (int i = 0; i < info.Length(); ++i)
          {
            if (Node::IsInstanceOf(isolate, info[i]))
            {
              auto nodeBinding = Node::Unwrap(isolate, info[i].As<Object>());
              if (nodeBinding != nullptr && nodeBinding->hasData())
              {
                nodes.push_back(nodeBinding->handle());
              }
            }
            else if (info[i]->IsString())
            {
              String::Utf8Value utf8Value(isolate, info[i]);
              shared_ptr<dom::Text> textNode = handle()->getOwnerDocumentChecked().createTextNode(string(*utf8Value));
              nodes.push_back(textNode);
            }
            else
            {
              isolate->ThrowException(Exception::TypeError(
                MakeMethodError(isolate, "after", "All arguments must be Node or string")));
              return;
            }
          }
          handle()->before(nodes);
          info.GetReturnValue().SetUndefined();
        }
      }

      void Element::CheckVisibility(const v8::FunctionCallbackInfo<v8::Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "checkVisibility", "Not implemented")));
      }

      void Element::Closest(const v8::FunctionCallbackInfo<v8::Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "closest", "Not implemented")));
      }

      void Element::GetAttribute(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          info.GetReturnValue().SetNull();
          return;
        }

        String::Utf8Value attrName(isolate, info[0]);
        string value = handle()->getAttribute(string(*attrName));
        info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                      value.c_str())
                                    .ToLocalChecked());
      }

      void Element::GetAttributeNames(const v8::FunctionCallbackInfo<v8::Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        auto attributeNames = handle()->getAttributeNames();
        Local<Array> result = Array::New(isolate, attributeNames.size());

        for (size_t i = 0; i < attributeNames.size(); ++i)
        {
          result->Set(isolate->GetCurrentContext(), static_cast<uint32_t>(i), String::NewFromUtf8(isolate, attributeNames[i].c_str()).ToLocalChecked())
            .Check();
        }

        info.GetReturnValue().Set(result);
      }

      void Element::GetBoundingClientRect(const v8::FunctionCallbackInfo<v8::Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        auto rect = handle()->getBoundingClientRect();
        Local<Object> result = Object::New(isolate);

        result->Set(isolate->GetCurrentContext(),
                    String::NewFromUtf8(isolate, "x").ToLocalChecked(),
                    Number::New(isolate, rect.x()))
          .Check();
        result->Set(isolate->GetCurrentContext(),
                    String::NewFromUtf8(isolate, "y").ToLocalChecked(),
                    Number::New(isolate, rect.y()))
          .Check();
        result->Set(isolate->GetCurrentContext(),
                    String::NewFromUtf8(isolate, "width").ToLocalChecked(),
                    Number::New(isolate, rect.width()))
          .Check();
        result->Set(isolate->GetCurrentContext(),
                    String::NewFromUtf8(isolate, "height").ToLocalChecked(),
                    Number::New(isolate, rect.height()))
          .Check();

        info.GetReturnValue().Set(result);
      }

      void Element::HasAttribute(const v8::FunctionCallbackInfo<v8::Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "hasAttribute", "1 argument required")));
          return;
        }
        if (!info[0]->IsString())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "hasAttribute", "Argument must be a string")));
          return;
        }

        String::Utf8Value attrName(isolate, info[0]);
        bool hasAttr = handle()->hasAttribute(string(*attrName));
        info.GetReturnValue().Set(Boolean::New(isolate, hasAttr));
      }

      void Element::HasAttributes(const v8::FunctionCallbackInfo<v8::Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        bool hasAttributes = handle()->hasAttributes();
        info.GetReturnValue().Set(Boolean::New(isolate, hasAttributes));
      }

      void Element::Remove(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        handle()->remove();
        info.GetReturnValue().SetNull();
      }

      void Element::RemoveAttribute(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "removeAttribute", "1 argument required")));
          return;
        }
        if (!info[0]->IsString())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "removeAttribute", "Argument must be a string")));
          return;
        }

        String::Utf8Value attrName(isolate, info[0]);
        handle()->removeAttribute(string(*attrName));
        info.GetReturnValue().SetNull();
      }

      void Element::SetAttribute(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "setAttribute", 2, info.Length())));
          return;
        }

        if (!info[0]->IsString())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "setAttribute", 0, "string", info[0])));
          return;
        }

        String::Utf8Value attrName(isolate, info[0]);
        String::Utf8Value attrValue(isolate,
                                    info[1]->ToString(context).ToLocalChecked());
        handle()->setAttribute(string(*attrName), string(*attrValue));
        info.GetReturnValue().SetNull();
      }

      void Element::SetAttributeNode(const v8::FunctionCallbackInfo<v8::Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "setAttributeNode", "Not implemented")));
      }

      void Element::SetAttributeNodeNS(const v8::FunctionCallbackInfo<v8::Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "setAttributeNodeNS", "Not implemented")));
      }

      void Element::SetAttributeNS(const v8::FunctionCallbackInfo<v8::Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "setAttributeNS", "Not implemented")));
      }

      void Element::GetElementsByTagName(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "getElementsByTagName", "Not implemented")));
      }

      void Element::GetElementsByClassName(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "getElementsByClassName", "Not implemented")));
      }

      void Element::QuerySelector(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "querySelector", "Not implemented")));
      }

      void Element::QuerySelectorAll(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "querySelectorAll", "Not implemented")));
      }
    }
  }
} // namespace endor
