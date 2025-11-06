#pragma once

#include <chrono>
#include <memory>
#include <glm/glm.hpp>
#include <common/xr/input_sources.hpp>

namespace endor
{
  namespace client_xr
  {
    // Forward declaration
    class XRInputSource;
    class XRSession;

    /**
     * Touchpad input mode
     */
    enum class TouchpadMode
    {
      Disabled = 0, // Touchpad input is disabled
      Scroll = 1,   // Scroll mode (default): touchpad slide emits WheelEvent
      Cursor = 2,   // Cursor mode (future): touchpad controls pointer position
    };

    /**
     * Configuration for touchpad input processing
     */
    struct TouchpadConfig
    {
      // Input mode
      TouchpadMode mode = TouchpadMode::Scroll;

      // Scroll mode settings
      bool inertiaEnabled = true;     // Enable kinetic inertia in scroll mode
      float decayConstantMs = 300.0f; // Exponential decay time constant (τ) in milliseconds
      float scrollScale = 40.0f;      // Scale factor for scroll deltas (px/unit)
      float deadzone = 0.01f;         // Deadzone for axes (0.0 to 1.0)

      // Scroll event throttling (matches DOM_SCROLLING_PERFORMANCE.md spec)
      std::chrono::milliseconds scrollThrottleDuration{16}; // 16ms = ~60fps

      // Future feature flags (disabled by default, placeholders for future implementation)
      bool doubleTapEnabled = false;  // Reserved: Enable double-tap detection
      bool longPressEnabled = false;  // Reserved: Enable long-press detection
      bool cursorModeEnabled = false; // Reserved: Enable cursor mode toggle
    };

    /**
     * Touchpad input processor
     * 
     * Processes touchpad input from XRInputSource and synthesizes DOM events.
     * Supports scroll mode with inertia by default.
     */
    class TouchpadInputProcessor
    {
    public:
      TouchpadInputProcessor(std::shared_ptr<XRSession> session, std::shared_ptr<XRInputSource> inputSource);

      /**
       * Update touchpad state and synthesize events
       * Called each frame from XRInputSourceArray::updateInputSources
       */
      void update(std::shared_ptr<class XRFrame> frame);

      /**
       * Get current configuration
       */
      const TouchpadConfig &config() const
      {
        return config_;
      }

      /**
       * Update configuration
       */
      void setConfig(const TouchpadConfig &config)
      {
        config_ = config;
      }

    private:
      // Process scroll mode input
      void processScrollMode(std::shared_ptr<class XRFrame> frame);

      // Apply inertia decay
      void applyInertia(float deltaTimeMs);

      // Synthesize WheelEvent to DOM
      void synthesizeWheelEvent(double deltaX, double deltaY);

      // Check if scroll event should be throttled
      bool shouldThrottleScrollEvent() const;

    private:
      std::shared_ptr<XRSession> session_;
      std::shared_ptr<XRInputSource> inputSource_;
      TouchpadConfig config_;

      // Previous state for delta calculation
      glm::vec2 previousAxes_{0.0f, 0.0f};
      bool wasTouched_ = false;

      // Inertia state
      glm::vec2 inertiaVelocity_{0.0f, 0.0f};
      std::chrono::steady_clock::time_point lastUpdateTime_;

      // Event throttling
      std::chrono::steady_clock::time_point lastScrollEventTime_;
    };
  }
} // namespace endor
