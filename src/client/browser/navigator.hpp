#pragma once

#include <memory>

namespace browser
{
  /**
   * @class Navigator
   * The Navigator class represents the navigator object that provides
   * information about the browser and the user's environment.
   */
  class Navigator
  {
  public:
    Navigator();
    ~Navigator() = default;

    /**
     * Returns the online status of the browser.
     * This property reflects the network connectivity status.
     */
    bool onLine() const;

    /**
     * Update the online status (called by network monitor)
     */
    void updateOnlineStatus(bool isOnline);

  private:
    bool online_status_ = true;
  };

  /**
   * Get the global navigator instance
   */
  Navigator& getNavigator();
} // namespace browser