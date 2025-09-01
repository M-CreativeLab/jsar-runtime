#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/builtin_scene/ecs.hpp>
#include <client/builtin_scene/ecs-inl.hpp>
#include <atomic>
#include <thread>
#include <chrono>

using namespace builtin_scene::ecs;

class TestComponent : public Component
{
public:
  TestComponent(int value) : value(value) {}

public:
  int value;
};

class TestComponent2 : public Component
{
public:
  TestComponent2(float value) : value(value) {}

public:
  float value;
};

class TestSystem : public System
{
public:
  using System::System;
  TestSystem(int id) : id(id) {}

  const std::string name() const override
  {
    return "TestSystem";
  }
  void onExecute() override
  {
    executed = true;
    std::cout << "TestSystem(" << id << ") executed." << std::endl;
  }

  bool executed = false;
  int id = 0;
};

class TestResource : public Resource
{
public:
  int value;
};

TEST_CASE("Entity creation", "[ecs]")
{
  auto app = std::make_shared<App>();
  app->registerComponent<TestComponent>();
  app->registerComponent<TestComponent2>();

  auto entity = app->spawn(TestComponent{42}, TestComponent2{3.14f});
  auto component = app->getComponent<TestComponent>(entity);
  auto component2 = app->getComponent<TestComponent2>(entity);

  REQUIRE(component != nullptr);
  REQUIRE(component->value == 42);
  REQUIRE(component2 != nullptr);
  REQUIRE(component2->value == 3.14f);
}

TEST_CASE("Component management", "[ecs]")
{
  auto app = std::make_shared<App>();
  app->registerComponent<TestComponent>();

  auto entity = app->spawn();
  app->addComponent(entity, TestComponent{42});
  auto component = app->getComponent<TestComponent>(entity);

  REQUIRE(component != nullptr);
  REQUIRE(component->value == 42);

  app->removeComponent<TestComponent>(entity);
  component = app->getComponent<TestComponent>(entity);

  REQUIRE(component == nullptr);
}

class Example : public App
{
public:
  using App::App;

public:
  inline void start()
  {
    update();
  }
};

TEST_CASE("System execution", "[ecs]")
{
  auto app = std::make_shared<Example>();
  auto system = System::Make<TestSystem>();

  app->addSystem(SchedulerLabel::kUpdate, system);
  app->start();

  REQUIRE(system->executed);
}

TEST_CASE("Resource management", "[ecs]")
{
  auto app = std::make_shared<App>();
  auto resource = Resource::Make<TestResource>();
  resource->value = 42;

  app->addResource(resource);
  auto retrievedResource = app->getResource<TestResource>();

  REQUIRE(retrievedResource != nullptr);
  REQUIRE(retrievedResource->value == 42);

  app->removeResource<TestResource>();
  retrievedResource = app->getResource<TestResource>();

  REQUIRE(retrievedResource == nullptr);
}

TEST_CASE("Add components dynamically via addComponent", "[ecs]")
{
  auto app = std::make_shared<App>();
  app->registerComponent<TestComponent>();
  app->registerComponent<TestComponent2>();

  auto entity = app->spawn();
  app->addComponent(entity, TestComponent{42});
  auto component = app->getComponent<TestComponent>(entity);

  REQUIRE(component != nullptr);
  REQUIRE(component->value == 42);

  app->addComponent(entity, TestComponent2{3.14f});
  auto component2 = app->getComponent<TestComponent2>(entity);

  REQUIRE(component2 != nullptr);
  REQUIRE(component2->value == 3.14f);
}

TEST_CASE("Test more system labels", "[ecs]")
{
  auto app = std::make_shared<Example>();
  auto systemFirst = System::Make<TestSystem>();
  auto systemPreUpdate = System::Make<TestSystem>();
  auto systemStateTransition = System::Make<TestSystem>();
  auto systemPostUpdate = System::Make<TestSystem>();
  auto systemLast = System::Make<TestSystem>();

  app->addSystem(SchedulerLabel::kFirst, systemFirst);
  app->addSystem(SchedulerLabel::kPreUpdate, systemPreUpdate);
  app->addSystem(SchedulerLabel::kStateTransition, systemStateTransition);
  app->addSystem(SchedulerLabel::kPostUpdate, systemPostUpdate);
  app->addSystem(SchedulerLabel::kLast, systemLast);

  app->start();

  REQUIRE(systemFirst->executed);
  REQUIRE(systemPreUpdate->executed);
  REQUIRE(systemStateTransition->executed);
  REQUIRE(systemPostUpdate->executed);
  REQUIRE(systemLast->executed);
}

TEST_CASE("Test chaining systems", "[ecs]")
{
  auto app = std::make_shared<Example>();
  auto systemFirst = System::Make<TestSystem>(10);
  auto systemSecond = System::Make<TestSystem>(11);
  auto systemThird = System::Make<TestSystem>(12);

  systemFirst->chain(systemSecond)->chain(systemThird);
  app->addSystem(SchedulerLabel::kUpdate, systemFirst);
  app->start();

  REQUIRE(systemFirst->executed);
  REQUIRE(systemSecond->executed);
  REQUIRE(systemThird->executed);
}

// Test systems for parallel execution
class ParallelTestSystem : public System
{
public:
  using System::System;
  ParallelTestSystem(int id, std::atomic<int>* counter, std::atomic<int>* maxConcurrent) 
    : id(id), counter(counter), maxConcurrent(maxConcurrent) {}

  const std::string name() const override
  {
    return "ParallelTestSystem_" + std::to_string(id);
  }
  
  void onExecute() override
  {
    // Increment active counter
    int current = counter->fetch_add(1) + 1;
    
    // Update max concurrent if needed
    int currentMax = maxConcurrent->load();
    while (current > currentMax && !maxConcurrent->compare_exchange_weak(currentMax, current)) {
      currentMax = maxConcurrent->load();
    }
    
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Decrement counter
    counter->fetch_sub(1);
    executed = true;
  }

  bool executed = false;
  int id = 0;
  std::atomic<int>* counter;
  std::atomic<int>* maxConcurrent;
};

TEST_CASE("Parallel system execution", "[ecs][parallel]")
{
  auto app = std::make_shared<Example>();
  
  // Test data for tracking concurrent execution
  std::atomic<int> activeCounter{0};
  std::atomic<int> maxConcurrent{0};
  
  // Create multiple test systems
  auto system1 = std::make_shared<ParallelTestSystem>(1, &activeCounter, &maxConcurrent);
  auto system2 = std::make_shared<ParallelTestSystem>(2, &activeCounter, &maxConcurrent);
  auto system3 = std::make_shared<ParallelTestSystem>(3, &activeCounter, &maxConcurrent);
  auto system4 = std::make_shared<ParallelTestSystem>(4, &activeCounter, &maxConcurrent);
  
  app->addSystem(SchedulerLabel::kUpdate, system1);
  app->addSystem(SchedulerLabel::kUpdate, system2);
  app->addSystem(SchedulerLabel::kUpdate, system3);
  app->addSystem(SchedulerLabel::kUpdate, system4);
  
  SECTION("Sequential execution (default)")
  {
    // Default should be sequential
    REQUIRE(app->isParallelSystemsEnabled() == false);
    
    app->start();
    
    // All systems should have executed
    REQUIRE(system1->executed);
    REQUIRE(system2->executed);
    REQUIRE(system3->executed);
    REQUIRE(system4->executed);
    
    // Max concurrent should be 1 for sequential execution
    REQUIRE(maxConcurrent.load() == 1);
  }
  
  SECTION("Parallel execution enabled")
  {
    app->setParallelSystemsEnabled(true);
    REQUIRE(app->isParallelSystemsEnabled() == true);
    
    app->start();
    
    // All systems should have executed
    REQUIRE(system1->executed);
    REQUIRE(system2->executed);
    REQUIRE(system3->executed);
    REQUIRE(system4->executed);
    
    // Max concurrent should be greater than 1 for parallel execution
    REQUIRE(maxConcurrent.load() > 1);
  }
}

TEST_CASE("Environment variable configuration", "[ecs][config]")
{
  SECTION("Parallel systems enabled via environment")
  {
    // Set environment variable
    std::setenv("JSAR_PARALLEL_SYSTEMS", "1", 1);
    std::setenv("JSAR_PARALLEL_RENDER", "true", 1);
    
    auto app = std::make_shared<App>();
    
    REQUIRE(app->isParallelSystemsEnabled() == true);
    REQUIRE(app->isParallelRenderEnabled() == true);
    
    // Clean up
    std::unsetenv("JSAR_PARALLEL_SYSTEMS");
    std::unsetenv("JSAR_PARALLEL_RENDER");
  }
  
  SECTION("Parallel execution disabled by default")
  {
    auto app = std::make_shared<App>();
    
    REQUIRE(app->isParallelSystemsEnabled() == false);
    REQUIRE(app->isParallelRenderEnabled() == false);
  }
}

TEST_CASE("Runtime configuration API", "[ecs][config]")
{
  auto app = std::make_shared<App>();
  
  // Initially disabled
  REQUIRE(app->isParallelSystemsEnabled() == false);
  REQUIRE(app->isParallelRenderEnabled() == false);
  
  // Enable parallel systems
  app->setParallelSystemsEnabled(true);
  REQUIRE(app->isParallelSystemsEnabled() == true);
  REQUIRE(app->isParallelRenderEnabled() == false); // Should not affect render
  
  // Enable parallel render
  app->setParallelRenderEnabled(true);
  REQUIRE(app->isParallelSystemsEnabled() == true);
  REQUIRE(app->isParallelRenderEnabled() == true);
  
  // Disable both
  app->setParallelSystemsEnabled(false);
  app->setParallelRenderEnabled(false);
  REQUIRE(app->isParallelSystemsEnabled() == false);
  REQUIRE(app->isParallelRenderEnabled() == false);
}
