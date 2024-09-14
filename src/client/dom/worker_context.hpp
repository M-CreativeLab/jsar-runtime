#pragma once

#include <functional>
#include <string>
#include <memory>
#include <v8.h>

#include "./dom_scripting.hpp"
#include "./runtime_context.hpp"

namespace dom
{
  class WorkerContext : public RuntimeContext
  {
  public:
    using RuntimeContext::RuntimeContext;

  public:
    void initialize() override
    {
      RuntimeContext::initialize();
      scriptingContext->makeWorkerContext();
    }
  };
}
