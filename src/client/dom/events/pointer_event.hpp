#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "./mouse_event.hpp"

namespace dom::events
{
  class PointerEvent : public MouseEvent
  {
    using MouseEvent::MouseEvent;

  public:
    enum PointerType : uint8_t
    {
      kMouse,
      kPen,
      kTouch,
    };

    static std::unique_ptr<PointerEvent> Click()
    {
      return std::make_unique<PointerEvent>(DOMEventConstructorType::kPointerEvent,
                                            DOMEventType::Click);
    }

    static std::unique_ptr<PointerEvent> PointerDown()
    {
      return std::make_unique<PointerEvent>(DOMEventConstructorType::kPointerEvent,
                                            DOMEventType::PointerDown);
    }

    static std::unique_ptr<PointerEvent> PointerUp()
    {
      return std::make_unique<PointerEvent>(DOMEventConstructorType::kPointerEvent,
                                            DOMEventType::PointerUp);
    }

    static std::unique_ptr<PointerEvent> PointerMove()
    {
      return std::make_unique<PointerEvent>(DOMEventConstructorType::kPointerEvent,
                                            DOMEventType::PointerMove);
    }

    static std::unique_ptr<PointerEvent> PointerOver()
    {
      return std::make_unique<PointerEvent>(DOMEventConstructorType::kPointerEvent,
                                            DOMEventType::PointerOver);
    }

    static std::unique_ptr<PointerEvent> PointerOut()
    {
      return std::make_unique<PointerEvent>(DOMEventConstructorType::kPointerEvent,
                                            DOMEventType::PointerOut);
    }

    static std::unique_ptr<PointerEvent> PointerEnter()
    {
      return std::make_unique<PointerEvent>(DOMEventConstructorType::kPointerEvent,
                                            DOMEventType::PointerEnter,
                                            dom::DOMEventInit{
                                                .bubbles = false,
                                                .cancelable = false,
                                                .composed = false,
                                            });
    }

    static std::unique_ptr<PointerEvent> PointerLeave()
    {
      return std::make_unique<PointerEvent>(DOMEventConstructorType::kPointerEvent,
                                            DOMEventType::PointerLeave,
                                            dom::DOMEventInit{
                                                .bubbles = false,
                                                .cancelable = false,
                                                .composed = false,
                                            });
    }

  public:
    int pointerId() const { return pointer_id_; }
    PointerType pointerType() const { return pointer_type_; }

  private:
    int pointer_id_;
    PointerType pointer_type_;
    bool is_primary_;
    float height_;
    float width_;

    // A clamped value to represent the pressure.
    float pressure_;
    float tangential_pressure_;
    long twist_;

  private: // Pen-related members
    float altitude_angle_;
    float azimuth_angle_;
    float tilt_x_;
    float tilt_y_;
  };
}
