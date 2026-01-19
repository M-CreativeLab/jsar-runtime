#pragma once

#include <functional>
#include <mutex>
#include <condition_variable>
#include <common/command_buffers/gpu/error.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>

namespace commandbuffers::gpu
{
  class ExecutionQueueBase : public GPUHandle
  {
  public:
    using Task = std::function<void()>;

    // Whether the execution queue has scheduled commands to be submitted or executing.
    bool hasScheduledCommands() const;

    // In the 'Normal' mode, currently recorded commands in the backend submitted in the next Tick.
    // However in the 'Passive' mode, the submission will be postponed as late as possible, for
    // example, until the client has explictly issued a submission.
    enum class SubmitMode
    {
      Normal,
      Passive
    };

    // Tracks whether we are in a submit to avoid submit reentrancy. Reentrancy could otherwise
    // happen when allocating resources or staging memory during submission (for workarounds, or
    // emulation) and the heuristics ask for an early submit to happen (which would cause a
    // submit-in-submit and many issues).
    bool inSubmit = false;

  protected:
    using GPUHandle::GPUHandle;

  private:
    // Backend specific wait for idle function.
    virtual MaybeError waitForIdleForDestructionImpl() = 0;

    // Indicates whether the backend has pending commands to be submitted as soon as possible.
    virtual bool hasPendingCommands() const = 0;

    std::mutex mMutex;
    std::condition_variable mCv;
    bool mCallingCallbacks = false;
    bool mWaitingForIdle = false;
    bool mAssumeCompleted = false;
  };
}
