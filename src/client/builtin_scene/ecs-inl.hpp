#pragma once

#include "./ecs.hpp"

namespace builtin_scene::ecs
{
  template <typename T>
  void PluginsManager::registerPlugin()
  {
    PluginName name = typeid(T).name();
    if (pluginIds_.find(name) != pluginIds_.end())
      throw std::runtime_error("Plugin(" + std::string(name) + ") already registered.");
    pluginIds_.insert({name, nextPluginId_});
    plugins_.insert({name, std::make_shared<T>()});
    nextPluginId_ += 1;
  }

  template <typename T>
  uint32_t PluginsManager::getPluginId()
  {
    PluginName name = typeid(T).name();
    if (pluginIds_.find(name) == pluginIds_.end())
      throw std::runtime_error("Plugin(" + std::string(name) + ") not found.");
    return pluginIds_[name];
  }

  template <typename T>
  std::shared_ptr<T> PluginsManager::getPlugin()
  {
    PluginName name = typeid(T).name();
    if (plugins_.find(name) == plugins_.end())
      throw std::runtime_error("Plugin(" + std::string(name) + ") not found.");
    return std::dynamic_pointer_cast<T>(plugins_[name]);
  }

  template <typename ResourceType>
  void ResourcesManager::addResource(std::shared_ptr<ResourceType> resource)
  {
    ResourceName name = typeid(ResourceType).name();
    if (resources_.find(name) != resources_.end())
      throw std::runtime_error("Resource(" + std::string(name) + ") already registered.");
    resources_.insert({name, resource});
  }

  template <typename ResourceType>
  void ResourcesManager::removeResource()
  {
    ResourceName name = typeid(ResourceType).name();
    if (resources_.find(name) == resources_.end())
      throw std::runtime_error("Resource(" + std::string(name) + ") not found.");
    resources_.erase(name);
  }

  template <typename ResourceType>
  std::shared_ptr<ResourceType> ResourcesManager::getResource()
  {
    if (resources_.empty())
      return nullptr;

    ResourceName name = typeid(ResourceType).name();
    if (resources_.find(name) == resources_.end())
      return nullptr;
    return std::dynamic_pointer_cast<ResourceType>(resources_[name]);
  }

  template <typename T>
  std::shared_ptr<T> ComponentSet<T>::insert(EntityId entity, std::shared_ptr<T> component)
  {
    if (entityToIndexMap_.find(entity) != entityToIndexMap_.end())
      throw std::runtime_error("Entity already has a component of this type.");

    auto index = size_;
    entityToIndexMap_[entity] = index;
    indexToEntityMap_[index] = entity;
    components_[index] = component;
    size_ += 1;
    return component;
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
    entityToComponentCache_.erase(entity);
    size_ -= 1;
  }

  template <typename T>
  std::shared_ptr<T> ComponentSet<T>::get(EntityId entity)
  {
    auto cachedComponent = entityToComponentCache_[entity];
    if (!cachedComponent.expired())
      return cachedComponent.lock();

    if (entityToIndexMap_.find(entity) == entityToIndexMap_.end())
      return nullptr;

    std::shared_ptr<T> component = components_[entityToIndexMap_[entity]];
    entityToComponentCache_[entity] = component; // TODO: support LRU cache.
    return component;
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

  template <typename... ComponentTypeList>
  EntityId App::spawn(ComponentTypeList... components)
  {
    std::unique_lock<std::shared_mutex> lock(mutexForEntities_);

    auto newEntity = std::make_shared<Entity>();
    EntityId entityId = newEntity->id();
    entities_.push_back(std::make_pair(entityId, newEntity));

    // Add the components to the entity.
    (componentsMgr_.addComponent(entityId, components), ...);
    return entityId;
  }

  template <typename ComponentType>
  std::vector<EntityId> App::queryEntities(std::function<bool(const ComponentType &)> filter)
  {
    std::shared_lock<std::shared_mutex> lock(mutexForEntities_);
    std::vector<EntityId> result;
    auto componentSet = componentsMgr_.getComponentSet<ComponentType>();
    for (auto &pair : entities_)
    {
      EntityId entityId = pair.first;
      if (componentSet->contains(entityId))
      {
        if (filter != nullptr)
        {
          std::shared_ptr<ComponentType> component = componentSet->get(entityId);
          assert(component != nullptr);
          if (filter(*component) == false)
            continue;
        }
        result.push_back(entityId);
      }
    }
    return result;
  }

  template <typename QueryComponentType, typename IncludeComponentType>
  std::vector<std::pair<EntityId, std::shared_ptr<IncludeComponentType>>> App::queryEntitiesWithComponent(
      std::function<bool(const QueryComponentType &)> filter)
  {
    std::shared_lock<std::shared_mutex> lock(mutexForEntities_);
    std::vector<std::pair<EntityId, std::shared_ptr<IncludeComponentType>>> result;
    auto queryComponentSet = componentsMgr_.getComponentSet<QueryComponentType>();
    auto includeComponentSet = componentsMgr_.getComponentSet<IncludeComponentType>();
    for (auto &pair : entities_)
    {
      EntityId entityId = pair.first;
      if (queryComponentSet->contains(entityId))
      {
        std::shared_ptr<QueryComponentType> queryComponent = queryComponentSet->get(entityId);
        assert(queryComponent != nullptr);

        std::shared_ptr<IncludeComponentType> includeComponent;
        if constexpr (std::is_same<QueryComponentType, IncludeComponentType>::value)
          includeComponent = queryComponent; // If the types are the same, use the query component.
        else
          includeComponent = includeComponentSet->get(entityId);

        // Check if the entity should be included.
        if (filter == nullptr || filter(*queryComponent) == true)
          result.push_back(std::make_pair(entityId, includeComponent));
      }
    }
    return result;
  }

  template <typename ComponentType>
  std::optional<EntityId> App::firstEntity()
  {
    std::shared_lock<std::shared_mutex> lock(mutexForEntities_);
    std::optional<EntityId> result = std::nullopt;
    auto componentSet = componentsMgr_.getComponentSet<ComponentType>();
    for (auto &pair : entities_)
    {
      EntityId entityId = pair.first;
      if (componentSet->contains(entityId))
      {
        result = entityId;
        break;
      }
    }
    return result;
  }

  template <typename QueryComponentType, typename IncludeComponentType>
  std::optional<IncludeComponentType> App::firstEntityWithComponent()
  {
    std::shared_lock<std::shared_mutex> lock(mutexForEntities_);
    std::optional<IncludeComponentType> result = std::nullopt;
    auto queryComponentSet = componentsMgr_.getComponentSet<QueryComponentType>();
    auto includeComponentSet = componentsMgr_.getComponentSet<IncludeComponentType>();
    for (auto &pair : entities_)
    {
      EntityId entityId = pair.first;
      if (queryComponentSet->contains(entityId))
      {
        IncludeComponentType includeComponent = includeComponentSet->get(entityId);
        result = includeComponent;
        break;
      }
    }
    return result;
  }

  template <typename ComponentType>
  std::shared_ptr<ComponentType> App::getComponent(EntityId entity)
  {
    std::shared_lock<std::shared_mutex> lock(mutexForEntities_);
    return componentsMgr_.getComponent<ComponentType>(entity);
  }

  template <typename... ComponentTypeList>
  void App::addComponent(EntityId entity, ComponentTypeList... components)
  {
    std::unique_lock<std::shared_mutex> lock(mutexForEntities_);
    (componentsMgr_.addComponent(entity, components), ...);
  }

  template <typename ComponentType>
  void App::removeComponent(EntityId entity, bool ignoreIfNotExists)
  {
    std::unique_lock<std::shared_mutex> lock(mutexForEntities_);
    componentsMgr_.removeComponent<ComponentType>(entity, ignoreIfNotExists);
  }

  template <typename ComponentType>
  std::shared_ptr<ComponentType> App::replaceComponent(EntityId entity, ComponentType component)
  {
    std::unique_lock<std::shared_mutex> lock(mutexForEntities_);
    return componentsMgr_.replaceComponent(entity, component);
  }
}
