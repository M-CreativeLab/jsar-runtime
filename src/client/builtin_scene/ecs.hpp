#pragma once

#include <memory>
#include <vector>
#include <idgen.hpp>

namespace builtin_scene::ecs
{
  /**
   * The labels used to identify the different scheduling events.
   *
   * By default, it will run the following schedules in the given order.
   *
   * On the first run of a scheduler, it will run:
   * 1. `kPreStartup`
   * 2. `kStartup`
   * 3. `kPostStartup`
   *
   * Then it will run:
   * 1. `kFirst`
   * 2. `kPreUpdate`
   * 3. `kStateTransition`
   * 4. `kUpdate`
   * 5. `kPostUpdate`
   * 6. `kLast`
   */
  enum class SchedulerLabel
  {
    kFirst = 0x9,
    kLast,
    kStartup,
    kUpdate,
    kPreStartup,
    kPreUpdate,
    kPostStartup,
    kPostUpdate,
    kStateTransition,
  };

  typedef uint32_t EntityId;
  typedef uint32_t SystemId;

  /**
   * The class for all entities in the ECS.
   */
  class Entity
  {
  public:
    Entity() : id_(idGen_.get()) {}
    ~Entity() = default;

  public:
    /**
     * @returns The Id of the entity.
     */
    inline EntityId id() { return id_; }

  private:
    EntityId id_;

  private:
    static thread_local TrIdGenerator idGen_;
  };

  /**
   * The base class for all components in the ECS.
   * 
   * Components are the data that is attached to entities.
   */
  class Component
  {
  public:
    Component() {}
    virtual ~Component() = default;
  };

  /**
   * The base class for all systems in the ECS.
   */
  class System
  {
  public:
    System() : next_(nullptr) {}
    virtual ~System() = default;

  public:
    /**
     * This method should be implemented by the derived class to execute the system.
     */
    virtual void onExecute() = 0;

  public:
    /**
     * @returns The Id of the system.
     */
    inline SystemId id() { return id_; }

  public:
    /**
     * Run the system once, it also runs the next system in the chain if it's configured.
     */
    void runOnce()
    {
      onExecute();
      if (next_ != nullptr)
        next_->runOnce();
    }
    /**
     * Add a system to the chain of this system, the former will be executed after the latter.
     *
     * @param next The next system to run.
     */
    std::shared_ptr<System> chain(std::shared_ptr<System> next)
    {
      next_ = next;
      return next;
    }

  private:
    SystemId id_;
    std::shared_ptr<System> next_ = nullptr;

  private:
    static thread_local TrIdGenerator idGen_;
  };

  class EntityWithComponents : public Entity
  {
  public:
    static std::shared_ptr<EntityWithComponents> Make(std::vector<Component> &components)
    {
      return std::make_shared<EntityWithComponents>(components);
    }

  public:
    EntityWithComponents(std::vector<Component> &components) : components(components)
    {
    }

  public:
    Entity entity;
    std::vector<Component> components;
  };

  /**
   * The main class for the ECS.
   */
  class App
  {
  public:
    App() {}

  public:
    /**
     * Spawn a new entity with the given components.
     * 
     * @param components The components to attach to the entity.
     * @returns The Id of the new entity.
     */
    EntityId spawn(std::vector<Component> components)
    {
      auto newEntity = EntityWithComponents::Make(components);
      auto newId = newEntity->id();
      entities_.push_back(std::make_pair(newId, newEntity));
      return newId;
    }
    /**
     * Add a system to the ECS with the given label.
     * 
     * @param label The label to use for scheduling the system.
     * @param system The system to add.
     * @returns The Id of the added system.
     */
    SystemId addSystem(SchedulerLabel label, std::shared_ptr<System> system)
    {
      systems_.push_back(std::make_pair(label, system));
      return system->id();
    }

  protected:
    /**
     * The derived class should call this method when the environment is ready to start the ECS.
     */
    void startup()
    {
      runSystems(SchedulerLabel::kPreStartup);
      runSystems(SchedulerLabel::kStartup);
      runSystems(SchedulerLabel::kPostStartup);
    }
    /**
     * The derived class should call this method to update the ECS.
     */
    void update()
    {
      runSystems(SchedulerLabel::kFirst);
      runSystems(SchedulerLabel::kPreUpdate);
      runSystems(SchedulerLabel::kStateTransition);
      runSystems(SchedulerLabel::kUpdate);
      runSystems(SchedulerLabel::kPostUpdate);
      runSystems(SchedulerLabel::kLast);
    }

  private:
    /**
     * Run all systems with the given label.
     */
    void runSystems(SchedulerLabel label)
    {
      for (auto &pair : systems_)
      {
        if (pair.first == label)
          pair.second->runOnce();
      }
    }

  private:
    std::vector<std::pair<EntityId, std::shared_ptr<EntityWithComponents>>> entities_;
    std::vector<std::pair<SchedulerLabel, std::shared_ptr<System>>> systems_;
  };
}
