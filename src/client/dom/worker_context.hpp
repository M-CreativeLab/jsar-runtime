#pragma once

#include <functional>
#include <string>
#include <memory>
#include <assert.h>
#include <v8.h>

#include "crates/jsar_jsbindings.h"
#include "./dom_scripting.hpp"
#include "./runtime_context.hpp"

namespace dom
{
  enum class WorkerCredentials
  {
    Omit,
    SameOrigin,
    Include,
  };

  class WorkerContext : public RuntimeContext
  {
  public:
    using RuntimeContext::RuntimeContext;

  public:
    void start(const std::string &url)
    {
      assert(scriptStarted == false);
      scriptStarted = true;

      script = scriptingContext->create(getSharedPtr(), baseURI, sourceType);
      auto requestUrl = crates::jsar::UrlHelper::CreateUrlStringWithPath(baseURI, url);
      fetchTextSourceResource(requestUrl, [this](const std::string &source)
                              {
        scriptingContext->compile(script, source);
        scriptingContext->evaluate(script);
        checkPendingEvents(); });
    }
    void startFromSource(const std::string &source)
    {
      assert(scriptStarted == false);
      scriptStarted = true;

      script = scriptingContext->create(getSharedPtr(), baseURI, sourceType);
      scriptingContext->compile(script, source);
      scriptingContext->evaluate(script);
      checkPendingEvents();
    }
    void dispatchEvent(v8::Local<v8::Object> event)
    {
      if (!scriptInitialized)
        pendingEventsToDispatch.push_back(v8::Global<v8::Object>(isolate, event));
      else
        scriptingContext->dispatchEvent(event);
    }

  private:
    void checkPendingEvents()
    {
      for (auto &eventStore : pendingEventsToDispatch)
      {
        auto event = v8::Local<v8::Object>::New(isolate, eventStore);
        scriptingContext->dispatchEvent(event);
      }
      pendingEventsToDispatch.clear();
      scriptInitialized = true;
    }

  public:
    std::string workerName;
    SourceTextType sourceType = SourceTextType::Classic;
    WorkerCredentials credentials = WorkerCredentials::SameOrigin;

  private:
    std::shared_ptr<DOMScript> script;
    bool scriptStarted = false;
    bool scriptInitialized = false;
    std::vector<v8::Global<v8::Object>> pendingEventsToDispatch;
  };
}
