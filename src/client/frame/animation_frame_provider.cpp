#include <chrono>
#include <client/per_process.hpp>
#include <idgen.hpp>

#include "./animation_frame_provider.hpp"

using namespace std;
using namespace std::chrono;

namespace endor
{
  namespace client_frame
  {
    AnimationFrameProvider::AnimationFrameProvider()
    {
      auto client_context = TrClientContextPerProcess::Get();
      event_loop_ = client_context->getScriptingEventLoop();
      assert(event_loop_ != nullptr && "Failed to start AnimationFrameProvider, event loop is null.");

      tick_handle_.data = this;
      uv_timer_init(event_loop_, &tick_handle_);
    }

    AnimationFrameProvider::~AnimationFrameProvider()
    {
      if (started_.load())
      {
        uv_timer_stop(&tick_handle_);
        started_ = false;
      }
    }

    void AnimationFrameProvider::start()
    {
      assert(!started_.load() && "AnimationFrameProvider is already started.");
      {
        auto timer_cb = [](uv_timer_t *handle)
        {
          auto self = static_cast<AnimationFrameProvider *>(handle->data);
          self->tick();
        };
        uv_timer_start(&tick_handle_, timer_cb, 0, 1);
      }
      started_ = true;
    }

    bool AnimationFrameProvider::isStarted() const
    {
      return started_.load();
    }

// TODO(yorkie): use the app frame rate to determine the threshold.
#define FRAME_TIME_DELTA_THRESHOLD 1000 / 45
    void AnimationFrameProvider::tick()
    {
      if (started_.load() == false) [[unlikely]]
        return;

      static steady_clock::time_point s_LastFrameTime = steady_clock::now();

      auto current_frame_time = steady_clock::now();
      auto delta = duration_cast<milliseconds>(current_frame_time - s_LastFrameTime).count();
      if (delta >= FRAME_TIME_DELTA_THRESHOLD)
      {
        s_LastFrameTime = current_frame_time;
        onFrameRequest(duration_cast<milliseconds>(current_frame_time.time_since_epoch()));
      }
    }

    void AnimationFrameProvider::onFrameRequest(chrono::milliseconds time)
    {
      vector<pair<long, AnimationFrameCallback>> pending_callbacks;
      {
        unique_lock lock(frame_callbacks_mutex_);
        pending_callbacks.swap(frame_callbacks_);
      }

      for (const auto &[id, callback] : pending_callbacks)
      {
        if (callback) [[likely]]
        {
          try
          {
            callback(static_cast<uint32_t>(time.count()));
          }
          catch (const std::exception &e)
          {
            // Log the exception but continue executing other callbacks
            cerr << "Exception in AnimationFrameCallback (id: " << id << "): " << e.what() << endl;
          }
          catch (...)
          {
            // Catch any other types of exceptions
            cerr << "Unknown exception in AnimationFrameCallback (id: " << id << ")" << endl;
          }
        }
      }
    }

    long AnimationFrameProvider::requestAnimationFrame(AnimationFrameCallback callback)
    {
      static TrIdGeneratorBase<long> s_CallbackIdGen(0x1);
      unique_lock lock(frame_callbacks_mutex_);

      long id = s_CallbackIdGen.get();
      frame_callbacks_.emplace_back(id, callback);
      return id;
    }

    void AnimationFrameProvider::cancelAnimationFrame(long id)
    {
      unique_lock lock(frame_callbacks_mutex_);
      frame_callbacks_.erase(
        remove_if(frame_callbacks_.begin(),
                  frame_callbacks_.end(),
                  [id](const pair<long, AnimationFrameCallback> &item)
                  { return item.first == id; }),
        frame_callbacks_.end());
    }
  }
} // namespace endor
