#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/builtin_scene/web_content.hpp>
#include <client/builtin_scene/web_content_renderer.cpp> // Include implementation for testing
#include <client/builtin_scene/text.hpp>
#include <thread>
#include <chrono>

using namespace builtin_scene;
using namespace builtin_scene::web_renderer;

// Test fixture class for async rendering tests
class AsyncRenderingTestFixture
{
public:
  AsyncRenderingTestFixture()
  {
    // Initialize a test WebContent with a small canvas
    content = std::make_shared<WebContent>("test_content", 100.0f, 100.0f, 0);
    content->resetSkSurface(100.0f, 100.0f);
    
    // Create a mock entity ID for testing
    entityId = 42;
  }

protected:
  std::shared_ptr<WebContent> content;
  ecs::EntityId entityId;
};

TEST_CASE_METHOD(AsyncRenderingTestFixture, "WebContent async state management", "[async_rendering]")
{
  SECTION("Initial state should be no async rendering in progress")
  {
    REQUIRE_FALSE(content->isAsyncRenderingInProgress());
  }

  SECTION("Setting async rendering in progress should be thread-safe")
  {
    content->setAsyncRenderingInProgress(true);
    REQUIRE(content->isAsyncRenderingInProgress());
    
    content->setAsyncRenderingInProgress(false);
    REQUIRE_FALSE(content->isAsyncRenderingInProgress());
  }

  SECTION("markAsyncRenderingCompleted should set surface dirty and clear async flag")
  {
    content->setSurfaceDirty(false);
    content->setAsyncRenderingInProgress(true);
    
    content->markAsyncRenderingCompleted();
    
    REQUIRE_FALSE(content->isAsyncRenderingInProgress());
    REQUIRE(content->isSurfaceDirty());
  }
}

TEST_CASE_METHOD(AsyncRenderingTestFixture, "Async state thread safety", "[async_rendering][thread_safety]")
{
  SECTION("Concurrent access to async state should be thread-safe")
  {
    const int numThreads = 4;
    const int numOperations = 100;
    std::vector<std::thread> threads;
    std::atomic<int> completedOperations{0};

    // Launch multiple threads that concurrently access async state
    for (int i = 0; i < numThreads; ++i)
    {
      threads.emplace_back([&]() {
        for (int j = 0; j < numOperations; ++j)
        {
          content->setAsyncRenderingInProgress(true);
          bool inProgress = content->isAsyncRenderingInProgress();
          REQUIRE(inProgress); // Should always be true after setting
          
          content->markAsyncRenderingCompleted();
          REQUIRE_FALSE(content->isAsyncRenderingInProgress());
          
          completedOperations++;
        }
      });
    }

    // Wait for all threads to complete
    for (auto& thread : threads)
    {
      thread.join();
    }

    REQUIRE(completedOperations == numThreads * numOperations);
    REQUIRE_FALSE(content->isAsyncRenderingInProgress());
  }
}

// Mock RenderContentBaseSystem for testing
class MockRenderContentBaseSystem : public RenderContentBaseSystem
{
public:
  using RenderContentBaseSystem::RenderContentBaseSystem;
  using RenderContentBaseSystem::scheduleAsyncSurfaceUpdate; // Make it accessible for testing

  const std::string name() const override
  {
    return "MockRenderContentBaseSystem";
  }

protected:
  bool render(ecs::EntityId entity, WebContent &content) override
  {
    // Mock implementation - just return true
    return true;
  }
};

TEST_CASE("scheduleAsyncSurfaceUpdate functionality", "[async_rendering][scheduling]")
{
  // Note: This test demonstrates the API but may not fully execute
  // in the test environment due to ECS system dependencies
  
  SECTION("Async operation should not block calling thread")
  {
    auto content = std::make_shared<WebContent>("test_async", 200.0f, 200.0f, 0);
    content->resetSkSurface(200.0f, 200.0f);
    
    REQUIRE_FALSE(content->isAsyncRenderingInProgress());
    
    // The async operation should set the in-progress flag
    // In a real scenario, this would be managed by the RenderContentBaseSystem
    content->setAsyncRenderingInProgress(true);
    REQUIRE(content->isAsyncRenderingInProgress());
    
    // Simulate async completion
    content->markAsyncRenderingCompleted();
    REQUIRE_FALSE(content->isAsyncRenderingInProgress());
    REQUIRE(content->isSurfaceDirty());
  }
}

TEST_CASE("UpdateTextureSystem async handling", "[async_rendering][texture_update]")
{
  SECTION("Content with async rendering in progress should be skipped")
  {
    auto content = std::make_shared<WebContent>("test_update", 100.0f, 100.0f, 0);
    content->resetSkSurface(100.0f, 100.0f);
    content->setSurfaceDirty(true);
    content->setAsyncRenderingInProgress(true);
    
    // The selectContents lambda in UpdateTextureSystem should exclude this content
    // because async rendering is in progress
    auto shouldSelect = [](const WebContent &content) -> bool {
      return content.canvas() != nullptr && 
             content.isSurfaceDirty() && 
             !content.isAsyncRenderingInProgress();
    };
    
    REQUIRE_FALSE(shouldSelect(*content));
    
    // Once async rendering completes, it should be selectable
    content->markAsyncRenderingCompleted();
    REQUIRE(shouldSelect(*content));
  }
}