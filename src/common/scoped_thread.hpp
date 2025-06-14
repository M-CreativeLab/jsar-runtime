#pragma once

#include <thread>
#include <atomic>
#include <common/debug.hpp>

class ScopedThread
{
public:
  ScopedThread() = default;
  virtual ~ScopedThread()
  {
    shutdown();
  }

public:
  void shutdown()
  {
    if (shutdownRequested)
      return;
    shutdownRequested = true;
    if (handle->joinable())
      handle->join();
  }

protected:
  std::unique_ptr<std::thread> handle;
  bool shutdownRequested = false;
};

/**
 * The `WorkerThread` class is a thread wrapper that does the work in a new thread, and it could be stopped, paused and resumed.
 */
class WorkerThread final : public ScopedThread
{
public:
  WorkerThread(std::string name, std::function<void(WorkerThread &)> work, uint32_t waitTime = 100)
      : ScopedThread()
      , name(name)
      , work(work)
      , waitTime(waitTime)
  {
    start();
  }

public:
  /**
   * Start the worker thread, it will start to do the work in a new thread.
   */
  void start()
  {
    if (started)
      return;
    running = true;
    handle = std::make_unique<std::thread>([this]()
                                           {
      SET_THREAD_NAME(name.c_str());
      /**
       * Sleep for 10ms to wait for the thread and instance to be ready.
       */
      std::this_thread::sleep_for(std::chrono::milliseconds(10));

      while (running)
      {
        if (work && !paused)
          work(*this);
        else
          sleep();
      } });
    started = true;
  }
  /**
   * Stop the worker thread, it will wait for the last work to finish and thread has been joined.
   */
  inline void stop()
  {
    running = false;
    shutdown();
  }
  /**
   * Pause the worker thread but it's alive, it will not do any work until the `resume` is called.
   */
  inline void pause()
  {
    paused = true;
  }
  /**
   * Resume the worker thread, it will continue to do the work.
   */
  inline void resume()
  {
    paused = false;
  }
  /**
   * Sleep the worker thread for a while(100ms).
   */
  void sleep()
  {
    if (handle->get_id() != std::this_thread::get_id())
      return;
    std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
  }

public:
  std::string name;
  std::atomic<bool> running = false;
  std::atomic<bool> paused = false;
  std::function<void(WorkerThread &)> work;

private:
  uint32_t waitTime = 100;
  bool started = false;
};
