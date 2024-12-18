#pragma once

#include <vector>
#include <memory>
#include "./common.hpp"

namespace client_xr
{
  /**
   * The `XRHand` interface is pair iterator with the key being the hand joints and the value being an `XRJointSpace`.
   */
  class XRHand : public std::vector<std::shared_ptr<XRJointSpace>>
  {
  public:
    XRHand(std::shared_ptr<XRInputSource> inputSource);
    XRHand(XRHand &that);

  public:
    /**
     * It returns a `XRJointSpace` for a given hand joint index or `nullptr` if no such hand joint key is in the map.
     *
     * @param index The hand joint index.
     * @returns The `XRJointSpace` for the given hand joint index or `nullptr` if no such hand joint key is in the map.
     */
    std::shared_ptr<XRJointSpace> get(client_xr::XRJointIndex index);
    /**
     * It returns a `XRJointSpace` for a given hand joint key or `nullptr` if no such hand joint key is in the map.
     *
     * @param key The hand joint key.
     * @returns The `XRJointSpace` for the given hand joint key or `nullptr` if no such hand joint key is in the map.
     */
    std::shared_ptr<XRJointSpace> get(const std::string key);
    /**
     * @returns an array with all the hand joint keys.
     */
    inline std::vector<std::string> keys();
    /**
     * @returns an array with all the `XRJointSpace` values.
     */
    inline std::vector<std::shared_ptr<XRJointSpace>> values() { return *this; }

  private:
    XRInputSource *inputSource_;
  };
}
