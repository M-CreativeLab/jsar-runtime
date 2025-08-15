#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <client/per_process.hpp>
#include <client/dom/dom_event_target.hpp>
#include <common/events_v2/native_event.hpp>

namespace browser
{
  /**
   * @class Navigator
   * The `Navigator` class provides access to browser and system information,
   * and handles RPC calls and document events.
   */
  class Navigator : public dom::DOMEventTarget,
                    public std::enable_shared_from_this<Navigator>
  {
  public:
    Navigator(TrClientContextPerProcess *clientContext = TrClientContextPerProcess::Get());
    ~Navigator();

  public: // Standard Navigator properties
    /**
     * Gets the user agent string.
     */
    const std::string &userAgent() const
    {
      return user_agent_;
    }

    /**
     * Gets the platform string.
     */
    const std::string &platform() const
    {
      return platform_;
    }

    /**
     * Gets the application name.
     */
    const std::string &appName() const
    {
      return app_name_;
    }

    /**
     * Gets the application version.
     */
    const std::string &appVersion() const
    {
      return app_version_;
    }

    /**
     * Gets the language.
     */
    const std::string &language() const
    {
      return language_;
    }

    /**
     * Gets whether Java is enabled.
     */
    bool javaEnabled() const
    {
      return false;
    }

    /**
     * Gets whether cookies are enabled.
     */
    bool cookieEnabled() const
    {
      return cookie_enabled_;
    }

    /**
     * Gets whether the browser is online.
     */
    bool onLine() const
    {
      return on_line_;
    }

    /**
     * Gets the number of logical processors available.
     */
    int hardwareConcurrency() const
    {
      return hardware_concurrency_;
    }

  public: // RPC functionality
    /**
     * Makes an RPC call to the host process.
     * 
     * @param method The method name to call.
     * @param args The arguments to pass to the method.
     * @param callback Callback function to handle the response.
     * @returns The request ID for tracking the response.
     */
    uint32_t makeRpcCall(const std::string &method,
                         const std::vector<std::string> &args,
                         std::function<void(bool success, const std::string &response)> callback = nullptr);

  public: // Document event handling
    /**
     * Handles document request events.
     */
    void handleDocumentRequest(const std::string &url, uint32_t documentId, bool disableCache = false);

  private:
    void onNativeEvent(events_comm::TrNativeEventType eventType, std::shared_ptr<events_comm::TrNativeEvent> event);
    void handleRpcResponse(uint32_t requestId, const std::string &response);
    void handleDocumentEvent(const std::string &eventData);

  private: // Properties
    std::string user_agent_;
    std::string platform_;
    std::string app_name_;
    std::string app_version_;
    std::string language_;
    bool cookie_enabled_;
    bool on_line_;
    int hardware_concurrency_;

  private: // RPC handling
    std::map<uint32_t, std::function<void(bool, const std::string &)>> rpc_callbacks_;
    uint32_t next_request_id_;

  private:
    TrClientContextPerProcess *client_context_;
    events_comm::TrNativeEventListener native_event_listener_;
  };
} // namespace browser