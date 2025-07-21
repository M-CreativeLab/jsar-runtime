#pragma once

#include <client/browser/navigator.hpp>
#include <client/dom/network_status_monitor.hpp>

namespace bindings
{
  /**
   * Initialize network events bindings
   * This sets up the C++ network monitoring and makes navigator.onLine available
   */
  void initializeNetworkBindings();

  /**
   * Get navigator.onLine value
   * This can be called from JavaScript bindings
   */
  bool getNavigatorOnLine();

  /**
   * Set window online event handler
   * This can be called from JavaScript bindings to set window.ononline
   */
  void setWindowOnlineHandler(std::function<void()> handler);

  /**
   * Set window offline event handler
   * This can be called from JavaScript bindings to set window.onoffline
   */
  void setWindowOfflineHandler(std::function<void()> handler);

  /**
   * Helper function to set the current window (internal use)
   */
  void setCurrentWindow(browser::Window* window);
} // namespace bindings