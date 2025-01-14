#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/builtin_scene/ecs.hpp>
#include <client/builtin_scene/ecs-inl.hpp>

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
