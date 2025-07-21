#include "./network_bindings.hpp"
#include <client/browser/window.hpp>

namespace bindings
{
  // Global window instance for event handling
  static browser::Window* g_currentWindow = nullptr;

  void initializeNetworkBindings()
  {
    // Initialize the network monitoring system
    dom::initializeNetworkMonitoring();
    
    // Ensure the navigator and network monitor are initialized
    auto& navigator = browser::getNavigator();
    auto& monitor = dom::getNetworkMonitor();
    
    // The monitor should already be running, but let's make sure
    monitor.startMonitoring();
  }

  bool getNavigatorOnLine()
  {
    auto& navigator = browser::getNavigator();
    return navigator.onLine();
  }

  void setWindowOnlineHandler(std::function<void()> handler)
  {
    // Note: This is a simplified approach
    // In a real implementation, you'd want to properly manage window instances
    if (g_currentWindow)
    {
      g_currentWindow->ononline = handler;
    }
  }

  void setWindowOfflineHandler(std::function<void()> handler)
  {
    // Note: This is a simplified approach
    // In a real implementation, you'd want to properly manage window instances
    if (g_currentWindow)
    {
      g_currentWindow->onoffline = handler;
    }
  }

  // Helper function to set the current window (would be called during window creation)
  void setCurrentWindow(browser::Window* window)
  {
    g_currentWindow = window;
  }
} // namespace bindings