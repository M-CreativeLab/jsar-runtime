#include "env.hpp"
#include "debug.hpp"

using namespace std;
using namespace bindings;

TransmuteEnvironment *TransmuteEnvironment::instance_;

TransmuteEnvironment *TransmuteEnvironment::GetInstance()
{
  if (instance_ == nullptr)
  {
    instance_ = new TransmuteEnvironment();
    DEBUG("transmute", "TransmuteEnvironment instance(%p) created.", instance_);
  }
  return instance_;
}

void TransmuteEnvironment::setRuntimeInit(const char *argJson)
{
  is_ready_ = true;
  runtime_init_ = string(argJson);
}

string TransmuteEnvironment::getRuntimeInit()
{
  return runtime_init_;
}

bool TransmuteEnvironment::isRuntimeAvailable()
{
  return runtime_available_;
}

string TransmuteEnvironment::getRuntimeVersions()
{
  return runtime_versions_;
}

void TransmuteEnvironment::markRuntimeAvailable(string runtimeVersions)
{
  std::unique_lock<std::mutex> lock(runtime_available_mtx_);
  runtime_versions_ = runtimeVersions;
  runtime_available_ = true;
  runtime_available_cv_.notify_all();
}

void TransmuteEnvironment::waitForRuntimeAvailable()
{
  std::unique_lock<std::mutex> lock(runtime_available_mtx_);
  while (runtime_available_ == false)
    runtime_available_cv_.wait(lock);
}

void TransmuteEnvironment::reset()
{
  runtime_init_ = "";
  runtime_available_ = false;
  DEBUG("transmute", "TransmuteEnvironment instance(%p) reset.", instance_);
}

TransmuteEnvironment::TransmuteEnvironment()
{
  reset();
}
