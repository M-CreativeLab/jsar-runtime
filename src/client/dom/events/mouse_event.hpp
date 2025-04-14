#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "./ui_event.hpp"

namespace dom::events
{
  class MouseEvent : public UIEvent
  {
    using UIEvent::UIEvent;

  public:
    enum EventTarget
    {
      kMouseEnter,
      kMouseLeave,
      kMouseOut,
      kMouseOver,
      kDragEnter,
      kDragLeave,
    };

    static std::unique_ptr<MouseEvent> MouseDown()
    {
      return std::make_unique<MouseEvent>(DOMEventConstructorType::kMouseEvent,
                                          DOMEventType::MouseDown);
    }

    static std::unique_ptr<MouseEvent> MouseUp()
    {
      return std::make_unique<MouseEvent>(DOMEventConstructorType::kMouseEvent,
                                          DOMEventType::MouseUp);
    }

    static std::unique_ptr<MouseEvent> MouseMove()
    {
      return std::make_unique<MouseEvent>(DOMEventConstructorType::kMouseEvent,
                                          DOMEventType::MouseMove);
    }

    static std::unique_ptr<MouseEvent> MouseOver()
    {
      return std::make_unique<MouseEvent>(DOMEventConstructorType::kMouseEvent,
                                          DOMEventType::MouseOver);
    }

    static std::unique_ptr<MouseEvent> MouseOut()
    {
      return std::make_unique<MouseEvent>(DOMEventConstructorType::kMouseEvent,
                                          DOMEventType::MouseOut);
    }

    static std::unique_ptr<MouseEvent> MouseEnter()
    {
      return std::make_unique<MouseEvent>(DOMEventConstructorType::kMouseEvent,
                                          DOMEventType::MouseEnter,
                                          dom::DOMEventInit{
                                              .bubbles = false,
                                              .cancelable = false,
                                              .composed = false,
                                          });
    }

    static std::unique_ptr<MouseEvent> MouseLeave()
    {
      return std::make_unique<MouseEvent>(DOMEventConstructorType::kMouseEvent,
                                          DOMEventType::MouseLeave,
                                          dom::DOMEventInit{
                                              .bubbles = false,
                                              .cancelable = false,
                                              .composed = false,
                                          });
    }

  public:
    EventTarget relatedTarget() const { return related_target_; }
    int button() const { return button_; }
    int buttons() const { return buttons_; }

    unsigned int clientX() const { return client_xy_.x; }
    unsigned int clientY() const { return client_xy_.y; }
    unsigned int movementX() const { return movement_xy_.x; }
    unsigned int movementY() const { return movement_xy_.y; }
    unsigned int offsetX() const { return offset_xy_.x; }
    unsigned int offsetY() const { return offset_xy_.y; }
    unsigned int pageX() const { return page_xy_.x; }
    unsigned int pageY() const { return page_xy_.y; }

    bool altKey() const { return alt_key_; }
    bool ctrlKey() const { return ctrl_key_; }
    bool metaKey() const { return meta_key_; }
    bool shiftKey() const { return shift_key_; }

    bool getModifierState(const std::string &key) const;

  private:
    EventTarget related_target_;
    int button_ = 0;
    int buttons_ = 0;

    glm::uvec2 client_xy_;
    glm::uvec2 movement_xy_;
    glm::uvec2 offset_xy_;
    glm::uvec2 page_xy_;

    bool alt_key_ = false;
    bool ctrl_key_ = false;
    bool meta_key_ = false;
    bool shift_key_ = false;
  };
}
