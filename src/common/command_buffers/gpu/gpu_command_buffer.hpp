#pragma once

#include <vector>
#include <memory>

#include "./gpu_base.hpp"
#include "./gpu_buffer.hpp"
#include "./gpu_pipeline.hpp"
#include "./gpu_commands.hpp"

namespace commandbuffers
{
  class GPUCommandBufferBase : public GPUHandle
  {
    friend class GPUComputePassEncoderBase;
    friend class GPURenderPassEncoderBase;

  public:
    GPUCommandBufferBase(std::optional<std::string> label)
        : GPUHandle(label.value_or("GPUCommandBuffer"))
        , commands_()
    {
    }
    GPUCommandBufferBase(std::optional<std::string> label, const GPUCommandBufferBase &source)
        : GPUHandle(label.value_or("GPUCommandBuffer"))
        , commands_(source.commands_)
    {
    }
    virtual ~GPUCommandBufferBase() = default;

  public:
    virtual void execute() = 0;

  private:
    template <typename T, typename... Args>
    void addCommand(Args &&...args)
    {
      auto command = std::make_shared<T>(std::forward<Args>(args)...);
      commands_.push_back(command);
    }

  protected:
    std::vector<std::shared_ptr<GPUCommand>> commands_;
  };
}
