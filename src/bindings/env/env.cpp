#include "env.hpp"
#include "debug.hpp"

using namespace std;
using namespace env;

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

void TransmuteEnvironment::SetReadyContext(const char *contextJson)
{
    ready_ = true;
    ready_context_ = string(contextJson);
}

string TransmuteEnvironment::GetReadyContext()
{
    return ready_context_;
}

bool TransmuteEnvironment::IsReady()
{
    return ready_;
}

bool TransmuteEnvironment::IsRuntimeAvailable()
{
    return runtime_available_;
}

void TransmuteEnvironment::MarkRuntimeAvailable()
{
    std::unique_lock<std::mutex> lock(runtime_available_mtx_);
    runtime_available_ = true;
    runtime_available_cv_.notify_all();
}

void TransmuteEnvironment::WaitRuntimeAvailable()
{
    std::unique_lock<std::mutex> lock(runtime_available_mtx_);
    while (runtime_available_ == false)
        runtime_available_cv_.wait(lock);
}

void TransmuteEnvironment::Reset()
{
    ready_ = false;
    ready_context_ = "";
    runtime_available_ = false;
    DEBUG("transmute", "TransmuteEnvironment instance(%p) reset.", instance_);
}

TransmuteEnvironment::TransmuteEnvironment()
{
    Reset();
}
