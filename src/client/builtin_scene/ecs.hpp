/**
 * The ECS (Entity Component System) for the built-in scene.
 *
 * Thanks to the following links for the code inspiration:
 * - https://bevyengine.org/
 * - https://austinmorlan.com/posts/entity_component_system/
 */

#pragma once

#include <assert.h>
#include <array>
#include <memory>
#include <vector>
#include <unordered_map>
#include <shared_mutex>
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
  typedef uint32_t ComponentId;
  typedef uint32_t SystemId;
  typedef const char *ComponentName;
  typedef const char *PluginName;
  typedef const char *ResourceName;

  constexpr EntityId MAX_ENTITY_ID = 10 * 10000;
  constexpr SystemId MAX_SYSTEM_ID = 1000;

  // Forward declarations.
  class App;
  class Plugin;
  class Resource;
  class Entity;
  class Component;
  class System;

  /**
   * The plugin is used to register some components and systems to the ECS app.
   */
  class Plugin
  {
    friend class PluginsManager;

  public:
    Plugin() = default;
    virtual ~Plugin() = default;

  protected:
    /**
     * Implement this method to register the components and systems to the ECS app.
     *
     * @param app The ECS app to register the components and systems to.
     */
    virtual void build(App &app) = 0;
  };

  /**
   * The class for managing all plugins in the ECS.
   */
  class PluginsManager
  {
  public:
    PluginsManager() = default;

  public:
    /**
     * Register a new plugin.
     *
     * @tparam T The type of the plugin.
     */
    template <typename T>
    void registerPlugin();

    /**
     * Get the Id of the plugin of the given type.
     *
     * @tparam T The type of the plugin.
     * @returns The Id of the plugin of the given type.
     */
    template <typename T>
    uint32_t getPluginId();

    /**
     * Get the plugin of the given type.
     *
     * @tparam T The type of the plugin.
     * @returns The plugin of the given type.
     */
    template <typename T>
    std::shared_ptr<T> getPlugin();

    /**
     * Build all registered plugins.
     *
     * @param app The ECS app to register the components and systems to.
     */
    inline void build(App &app)
    {
      for (auto &pair : plugins_)
      {
        auto plugin = pair.second;
        plugin->build(app);
      }
    }

  private:
    std::unordered_map<PluginName, uint8_t> pluginIds_{};
    std::unordered_map<PluginName, std::shared_ptr<Plugin>> plugins_{};
    uint8_t nextPluginId_ = 0;
  };

  /**
   * The base class for the resources which are used to store the global data such as meshes, textures, etc.
   */
  class Resource
  {
    friend class ResourcesManager;

  public:
    /**
     * Create a new instance of the specified resource type.
     *
     * @tparam ResourceType The type of the resource.
     * @tparam Args The types of the arguments to pass to the constructor of the resource.
     * @param args The arguments to pass to the constructor of the resource.
     * @returns The new instance of the specified resource type.
     */
    template <typename ResourceType, typename... Args>
    static inline std::shared_ptr<ResourceType> Make(Args... args)
    {
      return std::make_shared<ResourceType>(args...);
    }

  public:
    Resource() = default;
    virtual ~Resource() = default;
  };

  /**
   * The class for managing all resources in the ECS.
   */
  class ResourcesManager
  {
  public:
    ResourcesManager() = default;

  public:
    /**
     * Add a resource to the manager.
     *
     * @tparam ResourceType The type of the resource.
     * @param resource The resource to add.
     */
    template <typename ResourceType>
    void addResource(std::shared_ptr<ResourceType> resource);
    /**
     * Remove a resource from the manager.
     *
     * @tparam ResourceType The type of the resource.
     */
    template <typename ResourceType>
    void removeResource();

    /**
     * Get the resource of the given type.
     *
     * @tparam ResourceType The type of the resource.
     * @returns The resource of the given type.
     */
    template <typename ResourceType>
    std::shared_ptr<ResourceType> getResource();

  private:
    std::unordered_map<ResourceName, std::shared_ptr<Resource>> resources_{};
  };

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
    inline static thread_local TrIdGenerator idGen_ = TrIdGenerator(0, MAX_ENTITY_ID);
  };

  /**
   * The base class for all components in the ECS.
   *
   * Components are the data that is attached to entities.
   */
  class Component
  {
  public:
    Component() = default;
    virtual ~Component() = default;
  };

  /**
   * The interface for managing a set of components.
   */
  class IComponentSet
  {
  public:
    virtual ~IComponentSet() = default;

  public:
    virtual void onEntityDestroyed(EntityId entity) = 0;
  };

  /**
   * The template class for managing a set of components of the given type.
   *
   * @tparam T The type of the component.
   */
  template <typename T>
  class ComponentSet : public IComponentSet
  {
    friend class ComponentsManager;

  public:
    /**
     * Create a new instance of the `ComponentSet`.
     *
     * @returns The new instance of the `ComponentSet`.
     */
    static inline std::shared_ptr<ComponentSet<T>> Make()
    {
      return std::make_shared<ComponentSet>();
    }
    /**
     * Create a new instance of the `ComponentSet` with the given components.
     *
     * @param entity The entity to attach the components to.
     * @param components The components to attach.
     * @returns The new instance of the `ComponentSet`.
     */
    static inline std::shared_ptr<ComponentSet<T>> Make(EntityId entity, std::vector<T> &components)
    {
      return std::make_shared<ComponentSet>(entity, components);
    }

  public:
    ComponentSet()
    {
    }
    ComponentSet(EntityId entity, std::vector<T> &components)
    {
      for (auto &component : components)
        insert(entity, component);
    }

  public:
    /**
     * Insert a new component for the given entity.
     *
     * @param entity The entity to attach the component to.
     * @param component The component to attach.
     */
    std::shared_ptr<T> insert(EntityId entity, std::shared_ptr<T> component);
    /**
     * Insert a new component with the given arguments to construct the component, then attach it to the entity.
     *
     * @tparam InitialzingArgs The types of the arguments to construct the component.
     * @param entity The entity to attach the component to.
     * @param args The arguments to construct the component.
     */
    template <typename... InitialzingArgs>
    inline std::shared_ptr<T> insertWithArgs(EntityId entity, InitialzingArgs... args)
    {
      return insert(entity, std::make_shared<T>(args...));
    }
    /**
     * Remove the component of the given entity.
     *
     * @param entity The entity to remove the component from.
     */
    void remove(EntityId entity);
    /**
     * Get the component of the given entity.
     *
     * @param entity The entity to get the component for.
     * @returns The component of the given entity.
     * @throws std::runtime_error if the entity does not have a component of this type.
     */
    std::shared_ptr<T> get(EntityId entity);
    /**
     * Check if the component set contains the component of the given entity.
     *
     * @param entity The entity to check.
     * @returns `true` if the component set contains the component of the given entity, `false` otherwise.
     */
    inline bool contains(EntityId entity) { return entityToIndexMap_.find(entity) != entityToIndexMap_.end(); }
    /**
     * Replace the component of the given entity with the new component.
     *
     * If the component already exists, it will be removed and replaced with the new component.
     *
     * @param entity The entity to replace the component for.
     * @param newComponent The new component to replace.
     * @returns The replaced component.
     */
    inline std::shared_ptr<T> replace(EntityId entity, std::shared_ptr<T> newComponent)
    {
      if (newComponent == nullptr)
        throw std::runtime_error("The component to add is null.");
      if (contains(entity))
        remove(entity);
      return insert(entity, newComponent);
    }

  private:
    void onEntityDestroyed(EntityId entity) override;

  private:
    std::array<std::shared_ptr<T>, MAX_ENTITY_ID> components_;
    std::unordered_map<EntityId, size_t> entityToIndexMap_;
    std::unordered_map<size_t, EntityId> indexToEntityMap_;
    size_t size_ = 0;
  };

  /**
   * The class for managing all components in the ECS.
   */
  class ComponentsManager
  {
  public:
    ComponentsManager()
    {
    }

  public:
    /**
     * Register a new component type.
     *
     * @tparam ComponentType The type of the component.
     * @throws std::runtime_error if the component is already registered.
     */
    template <typename ComponentType>
    void registerComponent();

    /**
     * Get the Id of the component of the given type.
     *
     * @tparam ComponentType The type of the component.
     * @returns The Id of the component of the given type.
     */
    template <typename ComponentType>
    ComponentId getComponentId();

    /**
     * Add the component of the given type to the given entity.
     *
     * @tparam ComponentType The type of the component.
     * @param entity The entity to add the component to.
     * @param component The component to add.
     * @returns The added component.
     */
    template <typename ComponentType>
    inline std::shared_ptr<ComponentType> addComponent(EntityId entity, ComponentType component)
    {
      return getComponentSet<ComponentType>()->insert(entity, std::make_shared<ComponentType>(component));
    }

    /**
     * Add the component of the given type to the given entity, with the provided arguments to construct the component.
     *
     * @tparam ComponentType The type of the component.
     * @tparam InitialzingArgs The types of the arguments to construct the component.
     * @param entity The entity to add the component to.
     * @param args The arguments to construct the component.
     * @returns The added component.
     */
    template <typename ComponentType, typename... InitialzingArgs>
    inline std::shared_ptr<ComponentType> addComponentWithArgs(EntityId entity, InitialzingArgs... args)
    {
      return getComponentSet<ComponentType>()->insertWithArgs(entity, args...);
    }

    /**
     * Remove the component of the given type from the given entity.
     *
     * @tparam ComponentType The type of the component.
     * @param entity The entity to remove the component from.
     * @param ignoreIfNotExists If `true`, it will ignore if the component does not exist, otherwise it will throw an error.
     */
    template <typename ComponentType>
    inline void removeComponent(EntityId entity, bool ignoreIfNotExists = false)
    {
      auto componentSet = getComponentSet<ComponentType>();
      assert(componentSet != nullptr);

      if (ignoreIfNotExists == true && !componentSet->contains(entity))
        return;
      componentSet->remove(entity);
    }

    /**
     * Get the component of the given type for the given entity.
     *
     * @tparam ComponentType The type of the component.
     * @param entity The entity to get the component for.
     * @returns The component of the given type for the given entity.
     */
    template <typename ComponentType>
    inline std::shared_ptr<ComponentType> getComponent(EntityId entity)
    {
      return getComponentSet<ComponentType>()->get(entity);
    }

    /**
     * Replace the component of the given type for the given entity.
     *
     * If the component already exists, it will be removed and replaced with the new component.
     *
     * @tparam ComponentType The type of the component.
     * @param entity The entity to replace the component for.
     * @param component The component to replace.
     * @returns The replaced component.
     */
    template <typename ComponentType>
    inline std::shared_ptr<ComponentType> replaceComponent(EntityId entity, ComponentType component)
    {
      return getComponentSet<ComponentType>()->replace(entity, std::make_shared<ComponentType>(component));
    }

    /**
     * Notify the manager that the entity has been destroyed.
     *
     * @param entity The Id of the entity that has been destroyed.
     */
    inline void onEntityDestroyed(EntityId entity)
    {
      for (auto &pair : componentSets_)
      {
        auto componentSet = pair.second;
        componentSet->onEntityDestroyed(entity);
      }
    }

    /**
     * Get the `ComponentSet` for the given component type.
     *
     * @tparam ComponentType The type of the component.
     * @returns The `ComponentSet` for the given component type.
     */
    template <typename ComponentType>
    std::shared_ptr<ComponentSet<ComponentType>> getComponentSet()
    {
      ComponentName name = typeid(ComponentType).name();
      if (componentSets_.find(name) == componentSets_.end())
        throw std::runtime_error("ComponentSet(" + std::string(name) + ") not found.");
      return std::dynamic_pointer_cast<ComponentSet<ComponentType>>(componentSets_[name]);
    }

  private:
    std::unordered_map<ComponentName, ComponentId> componentIds_{};
    std::unordered_map<ComponentName, std::shared_ptr<IComponentSet>> componentSets_{};
    ComponentId nextComponentId_ = 0;
  };

  class ISystemSet
  {
  public:
    virtual ~ISystemSet() = default;

  public:
    /**
     * Add a new system to the set.
     *
     * @param system The system to add.
     * @returns `true` if the system is added, `false` otherwise.
     */
    bool addSystem(std::shared_ptr<System> system);
    /**
     * Remove the system of the given Id from the set.
     *
     * @param id The Id of the system to remove.
     * @returns `true` if the system is removed, `false` otherwise.
     */
    bool removeSystem(SystemId id);
    /**
     * Run all systems in the set once.
     *
     * This method should be called in the main loop of the app.
     */
    void run();

  protected:
    std::vector<std::shared_ptr<System>> systems_;
  };

  template <typename L>
  class LabeledSystemSet : public ISystemSet
  {
  public:
    LabeledSystemSet() {}
  };

  /**
   * The main class for the ECS.
   */
  class App : public std::enable_shared_from_this<App>
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
    template <typename... ComponentTypeList>
    EntityId spawn(ComponentTypeList... components);
    /**
     * Remove the entity from the ECS.
     * 
     * @param entity The entity to remove.
     * @returns `true` if the entity is removed, `false` otherwise.
     */
    bool removeEntity(EntityId entity);
    /**
     * Query all entities with the given component type.
     *
     * @tparam ComponentType The type of the component.
     * @returns The list of entity Ids with the given component type.
     */
    template <typename ComponentType>
    [[nodiscard]] std::vector<EntityId> queryEntities();
    /**
     * Query all entities with the given query component type and include the include component type.
     *
     * @tparam QueryComponentType The component type to query.
     * @tparam IncludeComponentType The component type to be included as the result.
     * @returns The list of components of the given type.
     */
    template <typename QueryComponentType, typename IncludeComponentType>
    [[nodiscard]] std::vector<IncludeComponentType> queryEntitiesWithComponent();
    /**
     * Get the first entity with the given component type, or an empty optional if not found.
     *
     * @tparam ComponentType The type of the component.
     * @returns The Id of the first entity with the given component type.
     */
    template <typename ComponentType>
    [[nodiscard]] std::optional<EntityId> firstEntity();
    /**
     * Get the first entity with the given component type, or an empty optional if not found.
     *
     * @tparam QueryComponentType The type of the query component.
     * @tparam IncludeComponentType The type of the include component.
     * @returns The include component of the first entity with the query component type.
     */
    template <typename QueryComponentType, typename IncludeComponentType>
    [[nodiscard]] std::optional<IncludeComponentType> firstEntityWithComponent();
    /**
     * Check if the entity has the component of the given type.
     *
     * @tparam ComponentType The type of the component.
     * @param entity The entity to get the component for.
     * @returns `true` if the entity has the component of the given type, `false` otherwise.
     */
    template <typename ComponentType>
    [[nodiscard]] inline bool hasComponent(EntityId entity) { return getComponent<ComponentType>(entity) != nullptr; }
    /**
     * Get the component of the given entity.
     *
     * @tparam ComponentType The type of the component.
     * @param entity The entity to get the component for.
     * @returns The component of the given type for the given entity or an empty optional if not found.
     */
    template <typename ComponentType>
    [[nodiscard]] std::shared_ptr<ComponentType> getComponent(EntityId entity);
    /**
     * Get the component reference of the given entity, it will expect the entity to have the component. If you are not sure
     * if the entity has the component, use `getComponent<T>` instead.
     *
     * @tparam ComponentType The type of the component.
     * @param entity The entity to get the component for.
     * @returns The component reference of the given entity.
     */
    template <typename ComponentType>
    [[nodiscard]] ComponentType &getComponentChecked(EntityId entity)
    {
      auto componentRef = getComponent<ComponentType>(entity);
      assert(componentRef != nullptr && "The entity does not have the component.");
      return *componentRef;
    }
    /**
     * Add component(s) to the entity.
     *
     * @tparam ComponentTypeList The types of the components to add.
     * @param entity The entity to add the components to.
     * @param components The components to add.
     */
    template <typename... ComponentTypeList>
    void addComponent(EntityId entity, ComponentTypeList... components);
    /**
     * Remove the components of the entity.
     *
     * @tparam ComponentType The type of the component.
     * @param entity The entity to remove the component from.
     * @param ignoreIfNotExists If `true`, it will ignore if the component does not exist, otherwise it will throw an error.
     * @returns The number of components removed.
     */
    template <typename ComponentType>
    void removeComponent(EntityId entity, bool ignoreIfNotExists = false);
    /**
     * Replace the current component of the entity with the new one.
     *
     * @tparam ComponentType The type of the component.
     * @param entity The entity to replace the component for.
     * @param component The new component to replace with.
     * @returns The replaced component.
     */
    template <typename ComponentType>
    std::shared_ptr<ComponentType> replaceComponent(EntityId entity, ComponentType component);
    /**
     * Register a new component type to use in the app.
     *
     * @tparam ComponentType The type of the component.
     */
    template <typename ComponentType>
    inline void registerComponent()
    {
      componentsMgr_.registerComponent<ComponentType>();
    }
    /**
     * Add a plugin to the app.
     *
     * @tparam PluginType The type of the plugin.
     */
    template <typename PluginType>
    inline void addPlugin()
    {
      pluginsMgr_.registerPlugin<PluginType>();
    }
    /**
     * Add a resource to the app.
     *
     * @tparam ResourceType The type of the resource.
     * @param Resource The resource to add.
     * @returns The added resource.
     */
    template <typename ResourceType>
    inline std::shared_ptr<ResourceType> addResource(std::shared_ptr<ResourceType> resource)
    {
      resourcesMgr_.addResource(resource);
      return resource;
    }
    /**
     * Remove a resource from the app.
     *
     * @tparam ResourceType The type of the resource.
     */
    template <typename ResourceType>
    inline void removeResource()
    {
      resourcesMgr_.removeResource<ResourceType>();
    }
    /**
     * Get the resource of the given type.
     *
     * @tparam ResourceType The type of the resource.
     * @returns The resource of the given type.
     */
    template <typename ResourceType>
    inline std::shared_ptr<ResourceType> getResource()
    {
      return resourcesMgr_.getResource<ResourceType>();
    }
    /**
     * Add a system to the ECS with the given label.
     *
     * @param label The label to use for scheduling the system.
     * @param system The system to add.
     * @returns The Id of the added system.
     * @throws std::runtime_error if the system is null.
     */
    SystemId addSystem(SchedulerLabel label, std::shared_ptr<System> system);
    /**
     * Remove a system from the ECS app.
     *
     * @param id The Id of the system to remove.
     */
    void removeSystem(SystemId id);

  protected:
    /**
     * The derived class should call this method when the environment is ready to start the ECS.
     */
    void startup();
    /**
     * The derived class should call this method to update the ECS.
     */
    void update();

  private:
    /**
     * Run all systems with the given label.
     *
     * @param label The label to use for scheduling the systems.
     */
    void runSystems(SchedulerLabel label);

  private:
    PluginsManager pluginsMgr_;
    ResourcesManager resourcesMgr_;
    ComponentsManager componentsMgr_;
    std::vector<std::pair<EntityId, std::shared_ptr<Entity>>> entities_;
    std::unordered_map<SchedulerLabel, std::shared_ptr<ISystemSet>> systemSets_{};
    // mutexes to make the ECS thread-safe.
    std::shared_mutex mutexForEntities_;
    std::shared_mutex mutexForSystems_;
  };

  /**
   * The base class for all systems in the ECS.
   */
  class System
  {
    friend class App;

  public:
    /**
     * Create a new instance of the specified system type.
     *
     * @tparam SystemType The type of the system.
     * @tparam Args The types of the arguments to pass to the constructor of the system.
     * @param args The arguments to pass to the constructor of the system.
     * @returns The new instance of the specified system type.
     */
    template <typename SystemType, typename... Args>
    static std::shared_ptr<SystemType> Make(Args... args)
    {
      return std::make_shared<SystemType>(args...);
    }

  public:
    System() : next_(nullptr) {}
    virtual ~System() = default;

  public:
    /**
     * @returns The name of the system.
     */
    virtual const std::string name() const = 0;
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
     * @returns The next system.
     */
    std::shared_ptr<System> chain(std::shared_ptr<System> next)
    {
      next_ = next;
      return next;
    }

  protected: // Methods for system implementations.
    /**
     * Spawn a new entity with the given components.
     *
     * @param components The components to attach to the entity.
     * @returns The Id of the new entity.
     */
    template <typename... ComponentTypeList>
    inline EntityId spawn(ComponentTypeList... components)
    {
      return connectedApp_->spawn(components...);
    }
    /**
     * Query all entities with the given component type.
     *
     * @tparam ComponentType The type of the component.
     * @returns The list of entity Ids with the given component type.
     */
    template <typename ComponentType>
    inline std::vector<EntityId> queryEntities() { return connectedApp_->queryEntities<ComponentType>(); }
    /**
     * Query all entities with the given query component type and include the include component type.
     *
     * @tparam QueryComponentType The component type to query.
     * @tparam IncludeComponentType The component type to be included as the result.
     * @returns The list of components of the given type.
     */
    template <typename QueryComponentType, typename IncludeComponentType>
    inline std::vector<IncludeComponentType> queryEntitiesWithComponent()
    {
      return connectedApp_->queryEntitiesWithComponent<QueryComponentType, IncludeComponentType>();
    }
    /**
     * Get the first entity with the given component type, or an empty optional if not found.
     *
     * @tparam ComponentType The type of the component.
     * @returns The Id of the first entity with the given component type.
     */
    template <typename ComponentType>
    inline std::optional<EntityId> firstEntity() { return connectedApp_->firstEntity<ComponentType>(); }
    /**
     * Get the first entity with the given component type, or an empty optional if not found.
     *
     * @tparam QueryComponentType The type of the query component.
     * @tparam IncludeComponentType The type of the include component.
     * @returns The include component of the first entity with the query component type.
     */
    template <typename QueryComponentType, typename IncludeComponentType>
    inline std::optional<IncludeComponentType> firstEntityWithComponent()
    {
      return connectedApp_->firstEntityWithComponent<QueryComponentType, IncludeComponentType>();
    }
    /**
     * Check if the entity has the component of the given type.
     *
     * @tparam ComponentType The type of the component.
     * @param entity The entity to get the component for.
     * @returns `true` if the entity has the component of the given type, `false` otherwise.
     */
    template <typename ComponentType>
    [[nodiscard]] inline bool hasComponent(EntityId entity)
    {
      return connectedApp_->hasComponent<ComponentType>(entity);
    }
    /**
     * Get the component of the given entity.
     *
     * @tparam ComponentType The type of the component.
     * @param entity The entity to get the component for.
     * @returns The component of the given type for the given entity or nullptr if not found.
     */
    template <typename ComponentType>
    [[nodiscard]] inline std::shared_ptr<ComponentType> getComponent(EntityId entity)
    {
      return connectedApp_->getComponent<ComponentType>(entity);
    }
    /**
     * Get the component reference of the given entity, it will expect the entity to have the component. If you are not sure
     * if the entity has the component, use `getComponent<T>` instead.
     *
     * @tparam ComponentType The type of the component.
     * @param entity The entity to get the component for.
     * @returns The component reference of the given entity.
     */
    template <typename ComponentType>
    [[nodiscard]] const ComponentType &getComponentChecked(EntityId entity)
    {
      return connectedApp_->getComponentChecked<ComponentType>(entity);
    }
    /**
     * Add component(s) to the entity.
     *
     * @tparam ComponentTypeList The types of the components to add.
     * @param entity The entity to add the components to.
     * @param components The components to add.
     */
    template <typename... ComponentTypeList>
    inline void addComponent(EntityId entity, ComponentTypeList... components)
    {
      (connectedApp_->addComponent(components), ...);
    }
    /**
     * Remove the components of the entity.
     *
     * @tparam ComponentType The type of the component.
     * @param entity The entity to remove the component from.
     */
    template <typename ComponentType>
    inline void removeComponent(EntityId entity)
    {
      connectedApp_->removeComponent<ComponentType>(entity);
    }
    /**
     * Replace the current component of the entity with the new one.
     *
     * If the component already exists, it will be removed and replaced with the new component.
     *
     * @tparam ComponentType The type of the component.
     * @param entity The entity to replace the component for.
     * @param component The new component to replace with.
     * @returns The replaced component.
     */
    template <typename ComponentType>
    inline std::shared_ptr<ComponentType> replaceComponent(EntityId entity, ComponentType component)
    {
      return connectedApp_->replaceComponent<ComponentType>(entity, component);
    }
    /**
     * Get the resource of the given type.
     *
     * @tparam ResourceType The type of the resource.
     * @returns The resource of the given type.
     */
    template <typename ResourceType>
    inline std::shared_ptr<ResourceType> getResource()
    {
      return connectedApp_->getResource<ResourceType>();
    }

  private:
    void connect(std::shared_ptr<App> app)
    {
      connectedApp_ = app;
      if (next_ != nullptr)
        next_->connect(app);
    }

  private:
    SystemId id_;
    std::shared_ptr<App> connectedApp_ = nullptr;
    std::shared_ptr<System> next_ = nullptr;

  private:
    inline static thread_local TrIdGenerator idGen_ = TrIdGenerator(0, MAX_SYSTEM_ID);
  };
}
