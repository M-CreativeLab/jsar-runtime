#include "./binding.hpp"
#include "./node.hpp"
#include "./element.hpp"
#include "./html_element.hpp"
#include "./html_div_element.hpp"
#include "./document.hpp"
#include "./text.hpp"
#include "./console.hpp"
#include "../event.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace dom
  {
    void Initialize(Isolate *isolate, Local<Object> global)
    {
      HandleScope scope(isolate);

      cout << "Initializing V8 DOM bindings..." << endl;

      // Initialize base classes first
      auto eventConstructor = script_bindings::Event::Initialize(isolate);
      auto nodeConstructor = Node::Initialize(isolate);
      auto elementConstructor = Element::Initialize(isolate);
      auto htmlElementConstructor = HTMLElement::Initialize(isolate);
      auto htmlDivElementConstructor = HTMLDivElement::Initialize(isolate);
      auto documentConstructor = Document::Initialize(isolate);
      auto textConstructor = Text::Initialize(isolate);
      auto consoleConstructor = Console::Initialize(isolate);

      // Set up global constructors
      Local<Context> context = isolate->GetCurrentContext();

      global->Set(context, String::NewFromUtf8(isolate, "Event").ToLocalChecked(), eventConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "Node").ToLocalChecked(), nodeConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "Element").ToLocalChecked(), elementConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "HTMLElement").ToLocalChecked(), htmlElementConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "HTMLDivElement").ToLocalChecked(), htmlDivElementConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "Document").ToLocalChecked(), documentConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "Text").ToLocalChecked(), textConstructor).Check();

      // Create and set global console object
      auto consoleObject = Console::CreateConsoleObject(isolate);
      global->Set(context, String::NewFromUtf8(isolate, "console").ToLocalChecked(), consoleObject).Check();

      cout << "V8 DOM bindings initialized successfully" << endl;
    }

    Local<Object> CreateDOMModule(Isolate *isolate)
    {
      EscapableHandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      // Create module object
      Local<Object> module = Object::New(isolate);

      // Add constructors to module
      auto eventConstructor = script_bindings::Event::Initialize(isolate);
      auto nodeConstructor = Node::Initialize(isolate);
      auto elementConstructor = Element::Initialize(isolate);
      auto htmlElementConstructor = HTMLElement::Initialize(isolate);
      auto documentConstructor = Document::Initialize(isolate);
      auto textConstructor = Text::Initialize(isolate);
      auto consoleConstructor = Console::Initialize(isolate);

      module->Set(context, String::NewFromUtf8(isolate, "Event").ToLocalChecked(), eventConstructor).Check();
      module->Set(context, String::NewFromUtf8(isolate, "Node").ToLocalChecked(), nodeConstructor).Check();
      module->Set(context, String::NewFromUtf8(isolate, "Element").ToLocalChecked(), elementConstructor).Check();
      module->Set(context, String::NewFromUtf8(isolate, "HTMLElement").ToLocalChecked(), htmlElementConstructor).Check();
      module->Set(context, String::NewFromUtf8(isolate, "Document").ToLocalChecked(), documentConstructor).Check();
      module->Set(context, String::NewFromUtf8(isolate, "Text").ToLocalChecked(), textConstructor).Check();
      module->Set(context, String::NewFromUtf8(isolate, "Console").ToLocalChecked(), consoleConstructor).Check();

      return scope.Escape(module);
    }
  }
}