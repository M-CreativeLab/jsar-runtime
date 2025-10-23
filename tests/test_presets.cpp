#define CATCH_CONFIG_MAIN
#include "./catch2/catch_amalgamated.hpp"
#include <client/per_process.hpp>
#include <client/browser/window.hpp>

using namespace std;

class TransmuteUnitTestListener : public Catch::EventListenerBase
{
  using Catch::EventListenerBase::EventListenerBase;

public:
  void testRunStarting(Catch::TestRunInfo const &testRunInfo) override
  {
    auto client_context = TrClientContextPerProcess::Create();
    // TODO(yorkie): remove next line when the window is constructed properly
    client_context->window = make_shared<browser::Window>(client_context);
  }
};

CATCH_REGISTER_LISTENER(TransmuteUnitTestListener);
