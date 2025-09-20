#include "./binding.hpp"
#include "./event_target.hpp"
#include "./node.hpp"
#include "./element.hpp"
#include "./html_element.hpp"
#include "./document.hpp"
#include "./character_data.hpp"
#include "./text.hpp"
#include "./console.hpp"
#include "../event.hpp"
#include "../html/html_div_element.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace dom
  {
    void Initialize(Isolate *isolate, Local<Context> context)
    {
      HandleScope scope(isolate);
      Local<Object> global = context->Global();

      cout << "Initializing V8 DOM bindings..." << endl;

      // Initialize base classes first
      auto eventConstructor = script_bindings::Event::Initialize(isolate);
      auto eventTargetConstructor = EventTarget::Initialize(isolate);
      auto nodeConstructor = Node::Initialize(isolate);
      auto elementConstructor = Element::Initialize(isolate);
      auto htmlElementConstructor = HTMLElement::Initialize(isolate);
      auto documentConstructor = Document::Initialize(isolate);
      auto characterDataConstructor = CharacterData::Initialize(isolate);
      auto textConstructor = Text::Initialize(isolate);
      auto consoleConstructor = Console::Initialize(isolate);

      // Initialize HTML elements
      auto htmlDivElementConstructor = script_bindings::html::HTMLDivElement::Initialize(isolate);

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
      auto consoleObject = Console::CreateConsoleObject(isolate);
      global->Set(context, String::NewFromUtf8(isolate, "console").ToLocalChecked(), consoleObject).Check();

      cout << "V8 DOM bindings initialized successfully" << endl;
    }
  }
}