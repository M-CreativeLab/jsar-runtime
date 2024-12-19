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

  void System::runOnce()
  {
    onExecute();
    if (next_ != nullptr)
      next_->runOnce();
  }

  std::shared_ptr<System> System::chain(std::shared_ptr<System> next)
  {
    next_ = next;
    return next;
  }

  template <typename... ComponentTypeList>
  EntityId System::spawn(ComponentTypeList... components)
  {
    return connectedApp_->spawn(components...);
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
}
