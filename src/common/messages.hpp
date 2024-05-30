#pragma once

#include <chrono>
#include "idgen.hpp"
#include "common/ipc.hpp"

using namespace std;

enum CustomEventType
{
  CUSTOM_EVENT_RPC_REQUEST = 0x100,
  CUSTOM_EVENT_RPC_RESPONSE = 0x101,
  CUSTOM_EVENT_MESSAGE = 0x200,
  CUSTOM_EVENT_ERROR = 0x210,
  CUSTOM_EVENT_CLOSED = 0x220,
};

constexpr int16_t TR_CUSTOM_EVENT_MAGIC = 0x71ff;
static TrIdGenerator eventIdGenerator = TrIdGenerator(0xf);
class CustomEvent
{
public:
  static bool Serialize(CustomEvent &event, void **outData, size_t *outSize)
  {
    *outSize = sizeof(uint32_t) +        // id
               sizeof(CustomEventType) + // type
               sizeof(uint32_t) +        // detail size
               event.size;               // detail
    *outData = malloc(*outSize);
    if (*outData == nullptr)
      return false;

    uint32_t *p = (uint32_t *)*outData;
    *p++ = event.id;
    *p++ = event.type;
    *p++ = event.size;
    memcpy(p, event.data, event.size);
    return true;
  }
  static CustomEvent *Deserialize(void *data, size_t size)
  {
    if (size < sizeof(uint32_t) + sizeof(CustomEventType) + sizeof(uint32_t))
      return nullptr;

    CustomEvent *event = new CustomEvent();
    uint32_t *p = (uint32_t *)data;
    event->id = *p++;
    event->type = (CustomEventType)*p++;
    event->size = *p++;
    if (size < sizeof(uint32_t) + sizeof(CustomEventType) + sizeof(uint32_t) + event->size)
    {
      delete event;
      return nullptr;
    }

    event->data = (char *)malloc(event->size);
    if (event->data == nullptr)
    {
      delete event;
      return nullptr;
    }
    memcpy(event->data, p, event->size);
    return event;
  }

public:
  CustomEvent() : type(CUSTOM_EVENT_MESSAGE), id(eventIdGenerator.get())
  {
  }
  CustomEvent(CustomEventType type, string detail) : type(type),
                                                     id(eventIdGenerator.get())
  {
    size = detail.size() + 1;
    data = (char *)malloc(size);
    memcpy(data, detail.c_str(), size);
  }
  ~CustomEvent()
  {
    if (data != nullptr)
      free(data);
  }

public:
  string detail() const
  {
    return string(data, size);
  }

public:
  uint32_t id;
  CustomEventType type;
  char *data = nullptr;
  size_t size = 0;
};

class TrEventSender : public ipc::TrChannelSender<CustomEvent>
{
public:
  TrEventSender(ipc::TrOneShotClient<CustomEvent> *client) : ipc::TrChannelSender<CustomEvent>(client)
  {
  }

public:
  bool sendEvent(CustomEvent &event)
  {
    void *data;
    size_t size;
    if (!CustomEvent::Serialize(event, &data, &size) || data == nullptr)
      return false;

    size_t headerSize = sizeof(TR_CUSTOM_EVENT_MAGIC) + sizeof(size);
    size_t bufferSize = headerSize + size;
    char *buffer = (char *)malloc(bufferSize);

    // append header
    memcpy(buffer, &TR_CUSTOM_EVENT_MAGIC, sizeof(TR_CUSTOM_EVENT_MAGIC));
    memcpy(buffer + sizeof(TR_CUSTOM_EVENT_MAGIC), &size, sizeof(size));

    // append data
    memcpy(buffer + headerSize, data, size);
    free(data);

    // send
    bool result = sendRaw(buffer, bufferSize);
    free(buffer);
    return result;
  }
};

class TrEventReceiver : public ipc::TrChannelReceiver<CustomEvent>
{
public:
  TrEventReceiver(ipc::TrOneShotClient<CustomEvent> *client) : ipc::TrChannelReceiver<CustomEvent>(client)
  {
  }

public:
  CustomEvent *recvEvent(int recvTimeout)
  {
    int16_t magic;
    if (!tryRecvRaw(&magic, sizeof(magic), recvTimeout) || magic != TR_CUSTOM_EVENT_MAGIC)
      return nullptr;

    size_t dataSize;
    if (!tryRecvRaw(&dataSize, sizeof(dataSize), recvTimeout))
      return nullptr;

    auto data = (char *)malloc(dataSize);
    if (!tryRecvRaw(data, dataSize, recvTimeout))
      return nullptr;

    CustomEvent *event = CustomEvent::Deserialize(data, dataSize);
    free(data);
    return event;
  }
};

class AnimationFrameRequest
{
public:
  AnimationFrameRequest()
  {
    auto now = chrono::system_clock::now();
    time = chrono::duration_cast<chrono::microseconds>(now.time_since_epoch()).count();
  }

public:
  uint32_t time;
};
