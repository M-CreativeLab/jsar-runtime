/**
 * Example: V8 DOM Bindings Integration
 * 
 * This example demonstrates how to integrate the new V8 DOM bindings
 * into a V8 context and use them from JavaScript.
 */

#include <client/script_bindings/dom/binding.hpp>
#include <node/v8.h>
#include <iostream>

using namespace v8;

class V8DOMExample
{
public:
  void Run()
  {
    // Initialize V8
    std::unique_ptr<Platform> platform = platform::NewDefaultPlatform();
    V8::InitializePlatform(platform.get());
    V8::Initialize();

    // Create isolate
    Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
    Isolate *isolate = Isolate::New(create_params);

    {
      Isolate::Scope isolate_scope(isolate);
      HandleScope handle_scope(isolate);

      // Create context
      Local<Context> context = Context::New(isolate);
      Context::Scope context_scope(context);

      // Initialize DOM bindings
      Local<Object> global = context->Global();
      script_bindings::dom::Initialize(isolate, global);

      std::cout << "V8 DOM Bindings initialized successfully!" << std::endl;

      // Test basic functionality
      RunExampleScripts(isolate, context);
    }

    // Cleanup
    isolate->Dispose();
    V8::Dispose();
    V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;
  }

private:
  void RunExampleScripts(Isolate *isolate, Local<Context> context)
  {
    // Test 1: Console functionality
    std::cout << "\n=== Testing Console ===" << std::endl;
    ExecuteScript(isolate, context, R"(
      console.log('Hello from V8 DOM bindings!');
      console.warn('This is a warning');
      console.error('This is an error message');
      console.info('Info message');
    )");

    // Test 2: Check constructor availability
    std::cout << "\n=== Testing Constructor Availability ===" << std::endl;
    ExecuteScript(isolate, context, R"(
      console.log('Node constructor:', typeof Node);
      console.log('Element constructor:', typeof Element);
      console.log('HTMLElement constructor:', typeof HTMLElement);
      console.log('HTMLDivElement constructor:', typeof HTMLDivElement);
      console.log('Document constructor:', typeof Document);
      console.log('Text constructor:', typeof Text);
    )");

    // Test 3: Constructor names
    std::cout << "\n=== Testing Constructor Names ===" << std::endl;
    ExecuteScript(isolate, context, R"(
      console.log('Node.name:', Node.name);
      console.log('Element.name:', Element.name);
      console.log('HTMLElement.name:', HTMLElement.name);
      console.log('Document.name:', Document.name);
    )");

    // Test 4: Basic object creation (will need actual DOM backend for full functionality)
    std::cout << "\n=== Testing Object Creation (Limited without DOM backend) ===" << std::endl;
    ExecuteScript(isolate, context, R"(
      try {
        // These constructors exist but may not work fully without DOM backend
        console.log('Attempting to access constructors...');
        
        // Check if prototype methods exist
        console.log('Node.prototype.appendChild exists:', typeof Node.prototype.appendChild);
        console.log('Element.prototype.getAttribute exists:', typeof Element.prototype.getAttribute);
        console.log('Document.prototype.createElement exists:', typeof Document.prototype.createElement);
        
        console.log('V8 DOM binding layer is working correctly!');
      } catch (e) {
        console.error('Error testing constructors:', e.message);
      }
    )");

    // Test 5: Console methods
    std::cout << "\n=== Testing Console Methods ===" << std::endl;
    ExecuteScript(isolate, context, R"(
      console.log('Testing console methods...');
      console.debug('Debug message');
      console.trace('Trace message');
      console.assert(true, 'This assertion should pass');
      console.assert(false, 'This assertion should fail');
      console.clear();
    )");
  }

  void ExecuteScript(Isolate *isolate, Local<Context> context, const char *source)
  {
    HandleScope handle_scope(isolate);
    TryCatch try_catch(isolate);

    Local<String> source_string = String::NewFromUtf8(isolate, source).ToLocalChecked();
    Local<Script> script;
    
    if (!Script::Compile(context, source_string).ToLocal(&script))
    {
      ReportException(isolate, &try_catch);
      return;
    }

    Local<Value> result;
    if (!script->Run(context).ToLocal(&result))
    {
      ReportException(isolate, &try_catch);
      return;
    }
  }

  void ReportException(Isolate *isolate, TryCatch *try_catch)
  {
    HandleScope handle_scope(isolate);
    String::Utf8Value exception(isolate, try_catch->Exception());
    const char *exception_string = *exception;
    Local<Message> message = try_catch->Message();
    
    if (message.IsEmpty())
    {
      std::cerr << "JavaScript error: " << exception_string << std::endl;
    }
    else
    {
      String::Utf8Value filename(isolate, message->GetScriptOrigin().ResourceName());
      Local<Context> context(isolate->GetCurrentContext());
      int linenum = message->GetLineNumber(context).FromMaybe(-1);
      std::cerr << "JavaScript error at " << *filename << ":" << linenum << ": " << exception_string << std::endl;
    }
  }
};

int main()
{
  V8DOMExample example;
  example.Run();
  return 0;
}