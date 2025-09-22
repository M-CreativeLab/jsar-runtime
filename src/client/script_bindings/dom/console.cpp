#include "./console.hpp"
#include <iostream>
#include <sstream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace dom_bindings
  {
    // static
    void Console::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Add console methods
      instanceTemplate->Set(String::NewFromUtf8(isolate, "log").ToLocalChecked(),
                            FunctionTemplate::New(isolate, Log));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "info").ToLocalChecked(),
                            FunctionTemplate::New(isolate, Info));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "warn").ToLocalChecked(),
                            FunctionTemplate::New(isolate, Warn));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "error").ToLocalChecked(),
                            FunctionTemplate::New(isolate, Error));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "debug").ToLocalChecked(),
                            FunctionTemplate::New(isolate, Debug));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "trace").ToLocalChecked(),
                            FunctionTemplate::New(isolate, Trace));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "assert").ToLocalChecked(),
                            FunctionTemplate::New(isolate, Assert));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "clear").ToLocalChecked(),
                            FunctionTemplate::New(isolate, Clear));
    }

    // static
    Local<Object> Console::CreateConsoleObject(Isolate *isolate)
    {
      EscapableHandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      // Create a plain object with console methods
      Local<Object> console = Object::New(isolate);

      console->Set(context, String::NewFromUtf8(isolate, "log").ToLocalChecked(), Function::New(context, Log).ToLocalChecked()).Check();

      console->Set(context, String::NewFromUtf8(isolate, "info").ToLocalChecked(), Function::New(context, Info).ToLocalChecked()).Check();

      console->Set(context, String::NewFromUtf8(isolate, "warn").ToLocalChecked(), Function::New(context, Warn).ToLocalChecked()).Check();

      console->Set(context, String::NewFromUtf8(isolate, "error").ToLocalChecked(), Function::New(context, Error).ToLocalChecked()).Check();

      console->Set(context, String::NewFromUtf8(isolate, "debug").ToLocalChecked(), Function::New(context, Debug).ToLocalChecked()).Check();

      console->Set(context, String::NewFromUtf8(isolate, "trace").ToLocalChecked(), Function::New(context, Trace).ToLocalChecked()).Check();

      console->Set(context, String::NewFromUtf8(isolate, "assert").ToLocalChecked(), Function::New(context, Assert).ToLocalChecked()).Check();

      console->Set(context, String::NewFromUtf8(isolate, "clear").ToLocalChecked(), Function::New(context, Clear).ToLocalChecked()).Check();

      return scope.Escape(console);
    }

    // static
    Local<Function> Console::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<Console, void>::Initialize(isolate);
    }

    Console::Console(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<Console, void>(isolate, args)
    {
    }

    // Console methods

    // static
    void Console::Log(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      ostringstream oss;
      for (int i = 0; i < info.Length(); ++i)
      {
        if (i > 0)
          oss << " ";
        oss << FormatValue(isolate, info[i]);
      }

      LogMessage("log", oss.str());
    }

    // static
    void Console::Info(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      ostringstream oss;
      for (int i = 0; i < info.Length(); ++i)
      {
        if (i > 0)
          oss << " ";
        oss << FormatValue(isolate, info[i]);
      }

      LogMessage("info", oss.str());
    }

    // static
    void Console::Warn(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      ostringstream oss;
      for (int i = 0; i < info.Length(); ++i)
      {
        if (i > 0)
          oss << " ";
        oss << FormatValue(isolate, info[i]);
      }

      LogMessage("warn", oss.str());
    }

    // static
    void Console::Error(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      ostringstream oss;
      for (int i = 0; i < info.Length(); ++i)
      {
        if (i > 0)
          oss << " ";
        oss << FormatValue(isolate, info[i]);
      }

      LogMessage("error", oss.str());
    }

    // static
    void Console::Debug(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      ostringstream oss;
      for (int i = 0; i < info.Length(); ++i)
      {
        if (i > 0)
          oss << " ";
        oss << FormatValue(isolate, info[i]);
      }

      LogMessage("debug", oss.str());
    }

    // static
    void Console::Trace(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      ostringstream oss;
      oss << "Trace:";
      for (int i = 0; i < info.Length(); ++i)
      {
        oss << " " << FormatValue(isolate, info[i]);
      }

      // TODO: Add actual stack trace
      LogMessage("trace", oss.str());
    }

    // static
    void Console::Assert(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        LogMessage("error", "Assertion failed");
        return;
      }

      bool condition = info[0]->BooleanValue(isolate);
      if (!condition)
      {
        ostringstream oss;
        oss << "Assertion failed";
        for (int i = 1; i < info.Length(); ++i)
        {
          oss << " " << FormatValue(isolate, info[i]);
        }
        LogMessage("error", oss.str());
      }
    }

    // static
    void Console::Clear(const FunctionCallbackInfo<Value> &info)
    {
      // TODO: Implement console clearing if needed
      LogMessage("info", "Console was cleared");
    }

    // Helper methods

    // static
    string Console::FormatValue(Isolate *isolate, Local<Value> value)
    {
      HandleScope scope(isolate);

      if (value->IsString())
      {
        String::Utf8Value utf8(isolate, value);
        return string(*utf8);
      }
      else if (value->IsNumber())
      {
        double num = value->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0);
        return to_string(num);
      }
      else if (value->IsBoolean())
      {
        bool b = value->BooleanValue(isolate);
        return b ? "true" : "false";
      }
      else if (value->IsNull())
      {
        return "null";
      }
      else if (value->IsUndefined())
      {
        return "undefined";
      }
      else if (value->IsObject())
      {
        // For objects, try to convert to string or return [object Object]
        Local<String> str = value->ToString(isolate->GetCurrentContext()).FromMaybe(Local<String>());
        if (!str.IsEmpty())
        {
          String::Utf8Value utf8(isolate, str);
          return string(*utf8);
        }
        return "[object Object]";
      }
      else
      {
        // Fallback for other types
        Local<String> str = value->ToString(isolate->GetCurrentContext()).FromMaybe(Local<String>());
        if (!str.IsEmpty())
        {
          String::Utf8Value utf8(isolate, str);
          return string(*utf8);
        }
        return "[unknown]";
      }
    }

    // static
    void Console::LogMessage(const char *level, const string &message)
    {
      // For now, just output to stdout/stderr
      // In a full implementation, this would integrate with CDP and logging infrastructure
      if (strcmp(level, "error") == 0 || strcmp(level, "warn") == 0)
      {
        cerr << "[" << level << "] " << message << endl;
      }
      else
      {
        cout << "[" << level << "] " << message << endl;
      }
    }
  }
}