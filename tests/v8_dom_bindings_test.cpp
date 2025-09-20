#include <client/script_bindings/dom/binding.hpp>
#include <gtest/gtest.h>
#include <node/v8.h>

using namespace v8;
using namespace script_bindings::dom;

class V8DOMBindingsTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Initialize V8 (simplified for testing)
    platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();

    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    isolate = v8::Isolate::New(create_params);

    {
      v8::Isolate::Scope isolate_scope(isolate);
      v8::HandleScope handle_scope(isolate);

      // Create a new context
      v8::Local<v8::Context> context = v8::Context::New(isolate);
      context_.Reset(isolate, context);

      v8::Context::Scope context_scope(context);

      // Initialize DOM bindings
      v8::Local<v8::Object> global = context->Global();
      script_bindings::dom::Initialize(isolate, global);
    }
  }

  void TearDown() override
  {
    context_.Reset();
    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;
  }

  v8::Local<v8::Value> RunScript(const char *source)
  {
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = context_.Get(isolate);
    v8::Context::Scope context_scope(context);

    v8::Local<v8::String> source_string = v8::String::NewFromUtf8(isolate, source).ToLocalChecked();
    v8::Local<v8::Script> script = v8::Script::Compile(context, source_string).ToLocalChecked();
    return script->Run(context).ToLocalChecked();
  }

  std::unique_ptr<v8::Platform> platform;
  v8::Isolate::CreateParams create_params;
  v8::Isolate *isolate;
  v8::Global<v8::Context> context_;
};

TEST_F(V8DOMBindingsTest, ConstructorsAreAvailable)
{
  // Test that DOM constructors are available in global scope
  v8::Local<v8::Value> result;

  result = RunScript("typeof Node");
  EXPECT_TRUE(result->IsString());
  v8::String::Utf8Value node_type(isolate, result);
  EXPECT_STREQ(*node_type, "function");

  result = RunScript("typeof Element");
  EXPECT_TRUE(result->IsString());
  v8::String::Utf8Value element_type(isolate, result);
  EXPECT_STREQ(*element_type, "function");

  result = RunScript("typeof HTMLElement");
  EXPECT_TRUE(result->IsString());
  v8::String::Utf8Value html_element_type(isolate, result);
  EXPECT_STREQ(*html_element_type, "function");

  result = RunScript("typeof Document");
  EXPECT_TRUE(result->IsString());
  v8::String::Utf8Value document_type(isolate, result);
  EXPECT_STREQ(*document_type, "function");
}

TEST_F(V8DOMBindingsTest, ConsoleIsAvailable)
{
  // Test that console object is available and has expected methods
  v8::Local<v8::Value> result;

  result = RunScript("typeof console");
  EXPECT_TRUE(result->IsString());
  v8::String::Utf8Value console_type(isolate, result);
  EXPECT_STREQ(*console_type, "object");

  result = RunScript("typeof console.log");
  EXPECT_TRUE(result->IsString());
  v8::String::Utf8Value log_type(isolate, result);
  EXPECT_STREQ(*log_type, "function");

  result = RunScript("typeof console.error");
  EXPECT_TRUE(result->IsString());
  v8::String::Utf8Value error_type(isolate, result);
  EXPECT_STREQ(*error_type, "function");
}

TEST_F(V8DOMBindingsTest, BasicNodeMethods)
{
  // Test basic node methods without requiring actual DOM backend
  // This tests the binding layer only

  // Test that methods exist and are callable
  RunScript("console.log('Testing DOM bindings...')");

  // These should not throw errors, even if they return null/undefined
  // due to missing DOM backend
  RunScript("var result = typeof Node.prototype.appendChild; result === 'function'");
  RunScript("var result = typeof Element.prototype.getAttribute; result === 'function'");
}

// Note: Full DOM functionality tests would require integrating with the actual
// DOM backend (dom::Document, dom::Element, etc.). These tests focus on
// verifying the V8 binding layer is correctly set up.