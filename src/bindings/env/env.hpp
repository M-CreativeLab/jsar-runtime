#pragma once

#include <mutex>
#include <condition_variable>
#include <napi.h>

using namespace std;

namespace bindings
{
  class TransmuteEnvironment
  {
  public:
    static TransmuteEnvironment *GetInstance();
    TransmuteEnvironment();

  public:
    void setRuntimeInit(const char *argJson);
    string getRuntimeInit();

    bool isRuntimeAvailable();
    string getRuntimeVersions();
    void markRuntimeAvailable(string runtimeVersions);
    void waitForRuntimeAvailable();
    void reset();

  private:
    bool is_ready_ = false;
    string runtime_init_;

    bool runtime_available_ = false;
    string runtime_versions_;
    std::mutex runtime_available_mtx_;
    std::condition_variable runtime_available_cv_;

    static TransmuteEnvironment *instance_;
  };
} // namespace bindings
