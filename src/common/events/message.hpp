#pragma once

#include <stdlib.h>
#include "./event.hpp"
#include "./event_type.hpp"

namespace events
{
  constexpr int16_t TR_EVENT_MAGIC = 0x71ff;

  class TrEventMessage
  {
  public:
    static bool Serialize(TrEventMessage &event, void **outData, size_t *outSize)
    {
      *outSize = sizeof(uint32_t) +    // id
                 sizeof(TrEventType) + // type
                 sizeof(uint32_t) +    // detail size
                 event.size;           // detail
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
    static TrEventMessage *Deserialize(void *data, size_t size)
    {
      if (size < sizeof(uint32_t) + sizeof(TrEventType) + sizeof(uint32_t))
        return nullptr;

      TrEventMessage *eventMessage = new TrEventMessage();
      uint32_t *p = (uint32_t *)data;
      eventMessage->id = *p++;
      eventMessage->type = (TrEventType)*p++;
      eventMessage->size = *p++;
      if (size < sizeof(uint32_t) + sizeof(TrEventType) + sizeof(uint32_t) + eventMessage->size)
      {
        delete eventMessage;
        return nullptr;
      }

      eventMessage->data = (char *)malloc(eventMessage->size);
      if (eventMessage->data == nullptr)
      {
        delete eventMessage;
        return nullptr;
      }
      memcpy(eventMessage->data, p, eventMessage->size);
      return eventMessage;
    }

  public:
    TrEventMessage() : type(TrEventType::TR_EVENT_MESSAGE), id(0)
    {
    }
    TrEventMessage(TrEvent &event) : type(event.type), id(event.id)
    {
      auto detailString = event.detail.getString();
      size = detailString.size() + 1;
      data = (char *)malloc(size);
      memcpy(data, detailString.c_str(), size);
    }
    ~TrEventMessage()
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
    TrEventType type;
    char *data = nullptr;
    size_t size = 0;
  };
}
