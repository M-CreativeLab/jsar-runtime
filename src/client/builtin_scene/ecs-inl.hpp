#include "./ecs.hpp"

namespace builtin_scene::ecs
{
  thread_local TrIdGenerator Entity::idGen_ = TrIdGenerator(0, MAX_ENTITY_ID);
  thread_local TrIdGenerator System::idGen_ = TrIdGenerator(0, MAX_SYSTEM_ID);

  template <typename T>
  void ComponentSet<T>::insert(EntityId entity, T component)
  {
    if (entityToIndexMap_.find(entity) != entityToIndexMap_.end())
      throw std::runtime_error("Entity already has a component of this type.");

    auto index = size_;
    entityToIndexMap_[entity] = index;
    indexToEntityMap_[index] = entity;
    components_[index] = component;
    size_ += 1;
  }

  template <typename T>
  void ComponentSet<T>::remove(EntityId entity)
  {
    if (entityToIndexMap_.find(entity) == entityToIndexMap_.end())
      throw std::runtime_error("Entity does not have a component of this type.");

    size_t indexToRemove = entityToIndexMap_[entity];
    size_t lastIndex = size_ - 1;
    components_[indexToRemove] = components_[lastIndex];

    EntityId lastEntity = indexToEntityMap_[lastIndex];
    entityToIndexMap_[lastEntity] = indexToRemove;
    indexToEntityMap_[indexToRemove] = lastEntity;

    entityToIndexMap_.erase(entity);
    indexToEntityMap_.erase(lastIndex);
    size_ -= 1;
  }

  template <typename T>
  T &ComponentSet<T>::get(EntityId entity)
  {
    if (entityToIndexMap_.find(entity) == entityToIndexMap_.end())
      throw std::runtime_error("Entity does not have a component of this type.");
    return components_[entityToIndexMap_[entity]];
  }

  template <typename T>
  void ComponentSet<T>::onEntityDestroyed(EntityId entity)
  {
    if (entityToIndexMap_.find(entity) != entityToIndexMap_.end())
      remove(entity);
  }

  template <typename ComponentType>
  void ComponentsManager::registerComponent()
  {
    ComponentName name = typeid(ComponentType).name();
    if (componentIds_.find(name) != componentIds_.end())
      throw std::runtime_error("Component(" + std::string(name) + ") already registered.");

    componentIds_.insert({name, nextComponentId_});
    componentSets_.insert({name, ComponentSet<ComponentType>::Make()});
    nextComponentId_ += 1;
  }

  template <typename ComponentType>
  ComponentId ComponentsManager::getComponentId()
  {
    ComponentName name = typeid(ComponentType).name();
    if (componentIds_.find(name) == componentIds_.end())
      throw std::runtime_error("Component(" + std::string(name) + ") not found.");
    return componentIds_[name];
  }

  void ComponentsManager::onEntityDestroyed(EntityId entity)
  {
    for (auto &pair : componentSets_)
      pair.second->onEntityDestroyed(entity);
  }

  bool ISystemSet::addSystem(std::shared_ptr<System> system)
  {
    systems_.push_back(system);
    return true;
  }

  bool ISystemSet::removeSystem(SystemId id)
  {
    for (auto it = systems_.begin(); it != systems_.end(); ++it)
    {
      if ((*it)->id() == id)
      {
        systems_.erase(it);
        return true;
      }
    }
    return false;
  }

  void ISystemSet::run()
  {
    for (auto &system : systems_)
      system->runOnce();
  }

  template <typename... ComponentTypeList>
  EntityId App::spawn(ComponentTypeList... components)
  {
    auto newEntity = std::make_shared<Entity>();
    EntityId entityId = newEntity->id();
    entities_.push_back(std::make_pair(entityId, newEntity));

    // Add the components to the entity.
    (componentsMgr_.addComponent(entityId, components), ...);
    return entityId;
  }

  template <typename ComponentType>
  std::vector<EntityId> App::queryEntities()
  {
    std::vector<EntityId> result;
    auto componentSet = componentsMgr_.getComponentSet<ComponentType>();
    for (auto &pair : entities_)
    {
      EntityId entityId = pair.first;
      if (componentSet->entityToIndexMap_.find(entityId) != componentSet->entityToIndexMap_.end())
        result.push_back(entityId);
    }
    return result;
  }

  template <typename QueryComponentType, typename IncludeComponentType>
  std::vector<IncludeComponentType> App::queryEntitiesWithComponent()
  {
    std::vector<IncludeComponentType> result;
    auto queryComponentSet = componentsMgr_.getComponentSet<QueryComponentType>();
    auto includeComponentSet = componentsMgr_.getComponentSet<IncludeComponentType>();
    for (auto &pair : entities_)
    {
      EntityId entityId = pair.first;
      if (queryComponentSet->entityToIndexMap_.find(entityId) != queryComponentSet->entityToIndexMap_.end())
      {
        IncludeComponentType includeComponent = includeComponentSet->get(entityId);
        result.push_back(includeComponent);
      }
    }
    return result;
  }

  SystemId App::addSystem(SchedulerLabel label, std::shared_ptr<System> system)
  {
    if (system == nullptr)
      throw std::runtime_error("System is null.");

    if (systemSets_.find(label) == systemSets_.end())
      systemSets_[label] = std::make_shared<LabeledSystemSet<SchedulerLabel>>();
    auto systemSet = std::static_pointer_cast<LabeledSystemSet<SchedulerLabel>>(systemSets_[label]);
    systemSet->addSystem(system);
    system->connect(shared_from_this());
    return system->id();
  }

  void App::removeSystem(SystemId id)
  {
    for (auto &pair : systemSets_)
    {
      auto systemSet = pair.second;
      systemSet->removeSystem(id);
    }
  }

  void App::startup()
  {
    runSystems(SchedulerLabel::kPreStartup);
    runSystems(SchedulerLabel::kStartup);
    runSystems(SchedulerLabel::kPostStartup);
  }

  void App::update()
  {
    runSystems(SchedulerLabel::kFirst);
    runSystems(SchedulerLabel::kPreUpdate);
    runSystems(SchedulerLabel::kStateTransition);
    runSystems(SchedulerLabel::kUpdate);
    runSystems(SchedulerLabel::kPostUpdate);
    runSystems(SchedulerLabel::kLast);
  }

  void App::runSystems(SchedulerLabel label)
  {
    if (systemSets_.find(label) == systemSets_.end())
      return;
    auto systemSet = systemSets_[label];
    systemSet->run();
  }
}
