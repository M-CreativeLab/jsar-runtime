#pragma once

#include <memory>
#include "./mouse_event.hpp"

namespace endor
{
  namespace dom::events
  {
    /**
     * The WheelEvent interface represents events that occur due to the user moving a mouse wheel
     * or similar input device.
     *
     * @see https://w3c.github.io/uievents/#interface-wheelevent
     */
    class WheelEvent : public MouseEvent
    {
      using MouseEvent::MouseEvent;

    public:
      // Delta mode constants
      static constexpr unsigned long DOM_DELTA_PIXEL = 0x00;
      static constexpr unsigned long DOM_DELTA_LINE = 0x01;
      static constexpr unsigned long DOM_DELTA_PAGE = 0x02;

      static std::unique_ptr<WheelEvent> Wheel()
      {
        return std::make_unique<WheelEvent>(DOMEventConstructorType::kWheelEvent,
                                            DOMEventType::Wheel);
      }

      static std::unique_ptr<WheelEvent> Create(double deltaX, double deltaY, double deltaZ = 0.0, unsigned long deltaMode = DOM_DELTA_PIXEL)
      {
        auto event = Wheel();
        event->delta_x_ = deltaX;
        event->delta_y_ = deltaY;
        event->delta_z_ = deltaZ;
        event->delta_mode_ = deltaMode;
        return event;
      }

    public:
      bool isWheelEvent() const override final
      {
        return true;
      }

      double deltaX() const
      {
        return delta_x_;
      }
      double deltaY() const
      {
        return delta_y_;
      }
      double deltaZ() const
      {
        return delta_z_;
      }
      unsigned long deltaMode() const
      {
        return delta_mode_;
      }

    private:
      double delta_x_ = 0.0;
      double delta_y_ = 0.0;
      double delta_z_ = 0.0;
      unsigned long delta_mode_ = DOM_DELTA_PIXEL;
    };
  }
} // namespace endor
