#include <iostream>
#include "./binding.hpp"
#include "./event.hpp"
#include "./event_target.hpp"
#include "./events/all_events.hpp"
#include "./dom/node.hpp"
#include "./dom/element.hpp"
#include "./dom/document.hpp"
#include "./dom/character_data.hpp"
#include "./dom/text.hpp"
#include "./dom/console.hpp"
#include "./html/html_element.hpp"
#include "./html/html_div_element.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  void Initialize(Isolate *isolate, Local<Context> context, ContextType type)
  {
    HandleScope scope(isolate);
    Local<Object> global = context->Global();

#define STRING_FROM_UTF8(str) String::NewFromUtf8(isolate, str).ToLocalChecked()

    // Initialize base classes first
    auto Event = script_bindings::Event::Initialize(isolate);
    auto EventTarget = script_bindings::EventTarget::Initialize(isolate);
    global->Set(context, STRING_FROM_UTF8("Event"), Event).Check();
    global->Set(context, STRING_FROM_UTF8("EventTarget"), EventTarget).Check();

    // Initialize event classes
    auto UIEvent = event_bindings::UIEvent::Initialize(isolate);
    auto MouseEvent = event_bindings::MouseEvent::Initialize(isolate);
    auto PointerEvent = event_bindings::PointerEvent::Initialize(isolate);
    global->Set(context, STRING_FROM_UTF8("UIEvent"), UIEvent).Check();
    global->Set(context, STRING_FROM_UTF8("MouseEvent"), MouseEvent).Check();
    global->Set(context, STRING_FROM_UTF8("PointerEvent"), PointerEvent).Check();

    // Create and set global console object
    auto Console = dom_bindings::Console::Initialize(isolate);
    auto console = dom_bindings::Console::CreateConsoleObject(isolate);
    global->Set(context, STRING_FROM_UTF8("Console"), Console).Check();
    global->Set(context, STRING_FROM_UTF8("console"), console).Check();

    // Initialize classes and objects for scripting context only
    if (type == ContextType::kScripting)
    {
      // Base DOM classes
      auto Node = dom_bindings::Node::Initialize(isolate);
      auto Element = dom_bindings::Element::Initialize(isolate);
      auto Document = dom_bindings::Document::Initialize(isolate);
      auto CharacterData = dom_bindings::CharacterData::Initialize(isolate);
      auto Text = dom_bindings::Text::Initialize(isolate);
      {
        global->Set(context, STRING_FROM_UTF8("Node"), Node).Check();
        global->Set(context, STRING_FROM_UTF8("Element"), Element).Check();
        global->Set(context, STRING_FROM_UTF8("Document"), Document).Check();
        global->Set(context, STRING_FROM_UTF8("CharacterData"), CharacterData).Check();
        global->Set(context, STRING_FROM_UTF8("Text"), Text).Check();
      }

      // HTML elements
      auto HTMLElement = html_bindings::HTMLElement::Initialize(isolate);
      auto HTMLDivElement = html_bindings::HTMLDivElement::Initialize(isolate);
      {
        global->Set(context, STRING_FROM_UTF8("HTMLElement"), HTMLElement).Check();
        global->Set(context, STRING_FROM_UTF8("HTMLDivElement"), HTMLDivElement).Check();
      }
    }

#undef STRING_FROM_UTF8
  }
}
