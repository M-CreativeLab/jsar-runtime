#pragma once

#include <mutex>
#include <condition_variable>
#include <napi.h>

using namespace std;

namespace env
{
  class TransmuteEnvironment
  {
  public:
    static TransmuteEnvironment *GetInstance();
    TransmuteEnvironment();

  public:
    void SetReadyContext(const char *contextJson);
    string GetReadyContext();
    bool IsReady();

    bool IsRuntimeAvailable();
    string GetRuntimeVersions();
    void MarkRuntimeAvailable(string runtimeVersions);
    void WaitRuntimeAvailable();
    void Reset();

  private:
    bool ready_ = false;
    string ready_context_;

    bool runtime_available_ = false;
    string runtime_versions_;
    std::mutex runtime_available_mtx_;
    std::condition_variable runtime_available_cv_;

    static TransmuteEnvironment *instance_;
  };
} // namespace env
