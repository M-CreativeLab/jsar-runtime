#include <cstring>
#include <stdlib.h>

#include "buffer_writter.hpp"
#include "debug.hpp"

using namespace gom;

std::map<std::string, GameObjectModelBufferWritter *> GameObjectModelBufferWritter::keyedBufferWrittersMap;

GameObjectModelBufferWritter::GameObjectModelBufferWritter()
{
}

GameObjectModelBufferWritter::~GameObjectModelBufferWritter()
{
  std::lock_guard<std::mutex> lock(mutex_);
  if (data_ != nullptr)
  {
    free(data_);
    data_ = nullptr;
  }
  data_size_ = 0;
  state_ = BufferWorkingState::Disposed;
  error_code_ = -1;
}

void GameObjectModelBufferWritter::setBuffer(void *data, size_t size)
{
  // Reset the data at first on writing this data everytime.
  resetBuffer();

  std::lock_guard<std::mutex> lock(mutex_);
  // Write the data to be loaded.
  data_size_ = size;
  data_ = (uint8_t *)malloc(size);
  memcpy(data_, data, size);
  // Set the state to be running.
  state_ = BufferWorkingState::Running;
  error_code_ = -1;
}

void GameObjectModelBufferWritter::setError(int code)
{
  if (code == -1)
  {
    DEBUG("transmute", "The error code is -1, which is not allowed.");
    return;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  state_ = BufferWorkingState::Errored;
  error_code_ = code;
}

int GameObjectModelBufferWritter::loadBuffer(uint8_t **out_data, size_t *out_size)
{
  std::lock_guard<std::mutex> lock(mutex_);
  if (data_ == nullptr || data_size_ == 0)
    return -1;

  *out_data = data_;
  *out_size = data_size_;
  return 0;
}

void GameObjectModelBufferWritter::resetBuffer()
{
  std::lock_guard<std::mutex> lock(mutex_);
  if (data_ != nullptr)
    free(data_);
  data_ = nullptr;
  data_size_ = 0;
  state_ = BufferWorkingState::Idle;
  error_code_ = -1;
}

BufferWorkingState GameObjectModelBufferWritter::bufferState()
{
  return state_;
}

int GameObjectModelBufferWritter::bufferSize()
{
  return data_size_;
}

void GameObjectModelBufferWritter::copyBuffer(uint8_t *target)
{
  if (data_ == nullptr)
    return;

  memcpy(target, data_, data_size_);
  free(data_);
  data_ = nullptr;
  data_size_ = 0;
}

void GameObjectModelBufferWritter::lockBuffer()
{
  mutex_.lock();
}

void GameObjectModelBufferWritter::unlockBuffer()
{
  mutex_.unlock();
}
