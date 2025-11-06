#include "./touchpad_input_processor.hpp"
#include "./webxr_input_sources.hpp"
#include "./webxr_session.hpp"
#include "./webxr_frame.hpp"
#include <client/dom/events/wheel_event.hpp>
#include <cmath>
#include <algorithm>

using namespace std;

namespace endor
{
  namespace client_xr
  {
    TouchpadInputProcessor::TouchpadInputProcessor(shared_ptr<XRSession> session,
                                                   shared_ptr<XRInputSource> inputSource)
        : session_(session)
        , inputSource_(inputSource)
        , lastUpdateTime_(chrono::steady_clock::now())
        , lastScrollEventTime_(chrono::steady_clock::now() - chrono::milliseconds(100))
    {
    }

    void TouchpadInputProcessor::update(shared_ptr<XRFrame> frame)
    {
      auto now = chrono::steady_clock::now();
      auto deltaTime = chrono::duration_cast<chrono::milliseconds>(now - lastUpdateTime_);
      float deltaTimeMs = static_cast<float>(deltaTime.count());
      lastUpdateTime_ = now;

      // Skip processing if mode is disabled
      if (config_.mode == TouchpadMode::Disabled)
      {
        return;
      }

      // Process based on current mode
      switch (config_.mode)
      {
      case TouchpadMode::Scroll:
        processScrollMode(frame);
        break;
      case TouchpadMode::Cursor:
        // Reserved for future implementation
        break;
      default:
        break;
      }

      // Apply inertia decay
      if (config_.inertiaEnabled && config_.mode == TouchpadMode::Scroll)
      {
        applyInertia(deltaTimeMs);
      }
    }

    void TouchpadInputProcessor::processScrollMode(shared_ptr<XRFrame> frame)
    {
      // Get gamepad state
      auto gamepad = inputSource_->gamepad();
      if (!gamepad)
      {
        return;
      }

      // Get touchpad axes (normalized -1 to 1)
      const auto &axes = gamepad->axes();
      if (axes.size() < 2)
      {
        return;
      }

      // Get button state
      const auto &buttons = gamepad->buttons();
      bool isTouched = buttons.size() > 0 && buttons[0].touched();

      // Apply deadzone
      float x = std::abs(axes[0]) < config_.deadzone ? 0.0f : axes[0];
      float y = std::abs(axes[1]) < config_.deadzone ? 0.0f : axes[1];
      glm::vec2 currentAxes(x, y);

      // Calculate deltas when touching
      if (isTouched && wasTouched_)
      {
        glm::vec2 delta = currentAxes - previousAxes_;

        // Early exit for zero offset (performance optimization per DOM_SCROLLING_PERFORMANCE.md)
        if (delta.x == 0.0f && delta.y == 0.0f)
        {
          previousAxes_ = currentAxes;
          return;
        }

        // Scale deltas to pixels
        double deltaX = static_cast<double>(delta.x * config_.scrollScale);
        double deltaY = static_cast<double>(delta.y * config_.scrollScale);

        // Clamp deltas to reasonable range (performance optimization)
        deltaX = std::clamp(deltaX, -1000.0, 1000.0);
        deltaY = std::clamp(deltaY, -1000.0, 1000.0);

        // Update inertia velocity if enabled
        if (config_.inertiaEnabled)
        {
          // Blend new velocity with existing inertia
          float alpha = 0.7f; // Blend factor
          inertiaVelocity_ = glm::mix(inertiaVelocity_, delta, alpha);
        }

        // Synthesize WheelEvent with throttling
        if (!shouldThrottleScrollEvent())
        {
          synthesizeWheelEvent(deltaX, deltaY);
          lastScrollEventTime_ = chrono::steady_clock::now();
        }
      }
      else if (!isTouched && wasTouched_)
      {
        // Finger lifted - start inertia decay if enabled
        // Inertia will be applied in applyInertia()
      }
      else if (isTouched && !wasTouched_)
      {
        // Finger touched - reset inertia
        inertiaVelocity_ = glm::vec2(0.0f, 0.0f);
      }

      // Update previous state
      previousAxes_ = currentAxes;
      wasTouched_ = isTouched;
    }

    void TouchpadInputProcessor::applyInertia(float deltaTimeMs)
    {
      // Skip if no inertia velocity
      if (inertiaVelocity_.x == 0.0f && inertiaVelocity_.y == 0.0f)
      {
        return;
      }

      // Skip if currently touching (inertia only applies when not touching)
      auto gamepad = inputSource_->gamepad();
      if (gamepad)
      {
        const auto &buttons = gamepad->buttons();
        if (buttons.size() > 0 && buttons[0].touched())
        {
          return;
        }
      }

      // Apply exponential decay: v(t) = v0 * e^(-t/τ)
      float decayFactor = std::exp(-deltaTimeMs / config_.decayConstantMs);
      inertiaVelocity_ *= decayFactor;

      // Stop inertia when velocity becomes negligible
      if (std::abs(inertiaVelocity_.x) < 0.001f && std::abs(inertiaVelocity_.y) < 0.001f)
      {
        inertiaVelocity_ = glm::vec2(0.0f, 0.0f);
        return;
      }

      // Synthesize scroll event from inertia
      double deltaX = static_cast<double>(inertiaVelocity_.x * config_.scrollScale);
      double deltaY = static_cast<double>(inertiaVelocity_.y * config_.scrollScale);

      // Clamp deltas
      deltaX = std::clamp(deltaX, -1000.0, 1000.0);
      deltaY = std::clamp(deltaY, -1000.0, 1000.0);

      if (!shouldThrottleScrollEvent())
      {
        synthesizeWheelEvent(deltaX, deltaY);
        lastScrollEventTime_ = chrono::steady_clock::now();
      }
    }

    void TouchpadInputProcessor::synthesizeWheelEvent(double deltaX, double deltaY)
    {
      // TODO: Implement actual event synthesis to DOM
      // For now, this is a placeholder that will be implemented when
      // we integrate with the DOM event system

      // The event should be dispatched to the appropriate DOM element
      // based on the current focus or hit-test result from the targetRay

      // Create WheelEvent with deltas
      auto wheelEvent = dom::events::WheelEvent::Create(
        deltaX, deltaY, 0.0, dom::events::WheelEvent::DOM_DELTA_PIXEL);

      // TODO: Dispatch to appropriate target element
      // This will require integration with:
      // - Hit testing system to find target element
      // - DOM event dispatch system
      // - XRSession's baseLayer or other rendering target
    }

    bool TouchpadInputProcessor::shouldThrottleScrollEvent() const
    {
      auto now = chrono::steady_clock::now();
      auto timeSinceLastEvent = chrono::duration_cast<chrono::milliseconds>(
        now - lastScrollEventTime_);
      return timeSinceLastEvent < config_.scrollThrottleDuration;
    }
  }
} // namespace endor
