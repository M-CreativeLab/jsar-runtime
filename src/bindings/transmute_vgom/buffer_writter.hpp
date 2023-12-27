#pragma once

#include <cstddef>
#include <string>
#include <map>
#include <mutex>

namespace gom
{
  enum BufferWorkingState
  {
    Idle = 0,
    Errored,
    Running,
  };

  class GameObjectModelBufferWritter
  {
  public:
    GameObjectModelBufferWritter();
    ~GameObjectModelBufferWritter();

  public:
    void setBuffer(void *data, size_t size);
    void setError(int error_code);
    int loadBuffer(uint8_t **out_data, size_t *out_size);
    void resetBuffer();

    BufferWorkingState bufferState();
    int bufferSize();
    void copyBuffer(uint8_t *target);
    void lockBuffer();
    void unlockBuffer();

  public:
    /**
     * @deprecated
     */
    static GameObjectModelBufferWritter *instance_;

    /**
     * This is to manage all the buffers
     */
    static std::map<std::string, GameObjectModelBufferWritter *> keyedBufferWrittersMap;

  private:
    uint8_t *data_ = nullptr;
    size_t data_size_ = 0;
    std::mutex mutex_;

    BufferWorkingState state_ = BufferWorkingState::Idle;
    int error_code_ = -1;
  };
}
