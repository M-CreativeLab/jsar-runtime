#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <vector>
#include <shared_mutex>
#include <node/uv.h>

namespace endor
{
  namespace client_frame
  {
    class AnimationFrameProvider
    {
      using AnimationFrameCallback = std::function<void(uint32_t time)>;

    public:
      AnimationFrameProvider();
      ~AnimationFrameProvider();

    public:
      void start();
      bool isStarted() const;
      void tick();
      void onFrameRequest(std::chrono::milliseconds time);

      long requestAnimationFrame(AnimationFrameCallback callback);
      void cancelAnimationFrame(long id);

    private:
      std::atomic<bool> started_ = false;
      uv_loop_t *event_loop_;
      uv_timer_t tick_handle_;

      std::shared_mutex frame_callbacks_mutex_;
      std::vector<std::pair<long, AnimationFrameCallback>> frame_callbacks_;
    };
  }
} // namespace endor
