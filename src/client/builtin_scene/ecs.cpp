#include "./ecs-inl.hpp"

#ifdef TR_ECS_ENABLE_TIME_PROFILING
#include <chrono>
#include <iomanip>
#endif

namespace builtin_scene::ecs
{
  using namespace std;
#ifdef TR_ECS_ENABLE_TIME_PROFILING
  using namespace std::chrono;
#endif

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

  bool App::removeEntity(EntityId entity)
  {
    unique_lock<shared_mutex> lock(mutexForEntities_);

    auto it = find_if(entities_.begin(), entities_.end(), [entity](const auto &pair)
                      { return pair.first == entity; });
    if (it == entities_.end())
      return false;

    componentsMgr_.onEntityDestroyed(entity);
    entities_.erase(it);
    return true;
  }

  SystemId App::addSystem(SchedulerLabel label, std::shared_ptr<System> system)
  {
    if (system == nullptr)
      throw std::runtime_error("System is null.");

    std::unique_lock<std::shared_mutex> lock(mutexForSystems_);
    if (systemSets_.find(label) == systemSets_.end())
      systemSets_[label] = std::make_shared<LabeledSystemSet<SchedulerLabel>>();
    auto systemSet = std::static_pointer_cast<LabeledSystemSet<SchedulerLabel>>(systemSets_[label]);
    systemSet->addSystem(system);
    system->connect(shared_from_this());
    return system->id();
  }

  void App::removeSystem(SystemId id)
  {
    std::unique_lock<std::shared_mutex> lock(mutexForSystems_);
    for (auto &pair : systemSets_)
    {
      auto systemSet = pair.second;
      systemSet->removeSystem(id);
    }
  }

  void App::startup()
  {
    // Build the plugins.
    pluginsMgr_.build(*this);

    // Schedule the "startup" systems.
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
    shared_lock<shared_mutex> lock(mutexForSystems_);
    if (systemSets_.find(label) == systemSets_.end())
      return;
    auto systemSet = systemSets_[label];
    systemSet->run();
  }

  void System::runOnce()
  {
#ifdef TR_ECS_ENABLE_TIME_PROFILING
    steady_clock::time_point started = chrono::high_resolution_clock::now();
#endif
    onExecute();

#ifdef TR_ECS_ENABLE_TIME_PROFILING
    steady_clock::time_point ended = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(ended - started);
    cout << fixed << setprecision(3);
    cout << "System(" << name() << ") took " << (duration.count() / 1000.0f) << "ms." << endl;

    // reset the format
    cout.unsetf(ios::fixed);
#endif

    if (next_ != nullptr)
      next_->runOnce();
  }

  void System::connect(shared_ptr<App> app)
  {
    connectedApp_ = app;
    if (next_ != nullptr)
      next_->connect(app);
  }
}
