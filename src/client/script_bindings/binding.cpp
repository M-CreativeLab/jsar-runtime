#include <iostream>
#include "./binding.hpp"
#include "./event.hpp"
#include "./event_target.hpp"
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

    // Initialize base classes first
    auto eventConstructor = script_bindings::Event::Initialize(isolate);
    auto eventTargetConstructor = script_bindings::EventTarget::Initialize(isolate);
    auto nodeConstructor = dom_bindings::Node::Initialize(isolate);
    auto elementConstructor = dom_bindings::Element::Initialize(isolate);
    auto documentConstructor = dom_bindings::Document::Initialize(isolate);
    auto characterDataConstructor = dom_bindings::CharacterData::Initialize(isolate);
    auto textConstructor = dom_bindings::Text::Initialize(isolate);
    auto consoleConstructor = dom_bindings::Console::Initialize(isolate);

    // Initialize HTML elements
    auto htmlElementConstructor = html_bindings::HTMLElement::Initialize(isolate);
    auto htmlDivElementConstructor = html_bindings::HTMLDivElement::Initialize(isolate);

    // Set up global constructors
    global->Set(context, String::NewFromUtf8(isolate, "Event").ToLocalChecked(), eventConstructor).Check();
    global->Set(context, String::NewFromUtf8(isolate, "EventTarget").ToLocalChecked(), eventTargetConstructor).Check();
    global->Set(context, String::NewFromUtf8(isolate, "Node").ToLocalChecked(), nodeConstructor).Check();
    global->Set(context, String::NewFromUtf8(isolate, "Element").ToLocalChecked(), elementConstructor).Check();
    global->Set(context, String::NewFromUtf8(isolate, "HTMLElement").ToLocalChecked(), htmlElementConstructor).Check();
    global->Set(context, String::NewFromUtf8(isolate, "HTMLDivElement").ToLocalChecked(), htmlDivElementConstructor).Check();
    global->Set(context, String::NewFromUtf8(isolate, "Document").ToLocalChecked(), documentConstructor).Check();
    global->Set(context, String::NewFromUtf8(isolate, "CharacterData").ToLocalChecked(), characterDataConstructor).Check();
    global->Set(context, String::NewFromUtf8(isolate, "Text").ToLocalChecked(), textConstructor).Check();

    // Create and set global console object
    auto consoleObject = dom_bindings::Console::CreateConsoleObject(isolate);
    global->Set(context, String::NewFromUtf8(isolate, "console").ToLocalChecked(), consoleObject).Check();

    cout << "V8 DOM bindings initialized successfully" << endl;
  }
}
