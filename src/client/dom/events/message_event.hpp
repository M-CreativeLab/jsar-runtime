#pragma once

#include <vector>
#include <span>
#include "../dom_event.hpp"
#include "../dom_event_target.hpp"

namespace endor
{
  namespace dom::events
  {
    class MessageEvent : public dom::Event
    {
      using dom::Event::Event;

    public:
      MessageEvent(const std::string &origin, uint8_t *data_buffer, size_t data_size)
          : dom::Event(DOMEventConstructorType::kMessageEvent, DOMEventType::Message)
          , origin(origin)
          , data_buffer_(data_buffer)
          , data_size_(data_size)
      {
      }
      ~MessageEvent()
      {
        if (data_buffer_ != nullptr)
        {
          delete[] data_buffer_;
          data_buffer_ = nullptr;
          data_size_ = 0;
        }
      }

      bool isMessageEvent() const override final
      {
        return true;
      }

    public:
      std::string origin;
      std::string lastEventId;
      const std::span<uint8_t> data() const
      {
        return std::span<uint8_t>(data_buffer_, data_size_);
      }

    protected:
      uint8_t *data_buffer_ = nullptr;
      size_t data_size_ = 0;
    };
  }
} // namespace endor
