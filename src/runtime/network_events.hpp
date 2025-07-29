#pragma once

#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <common/events_v2/event.hpp>
#include "network_monitor.hpp"

namespace runtime
{
  /**
   * @enum NetworkEventType
   * Types of network-related events that can be dispatched.
   */
  enum class NetworkEventType
  {
    Online, // Network connectivity is available
    Offline // Network connectivity is lost
  };

  /**
   * @class NetworkEventDetail
   * Detail object for network events containing additional context.
   */
  class NetworkEventDetail : public events_comm::TrEventDetailObject
  {
  public:
    NetworkEventDetail() = default;
    NetworkEventDetail(NetworkStatus status)
        : networkStatus(status)
    {
    }

  public:
    void serialize(rapidjson::Document &destDoc) override
    {
      auto &allocator = destDoc.GetAllocator();
      destDoc.AddMember("networkStatus",
                        static_cast<int>(networkStatus),
                        allocator);
      destDoc.AddMember("timestamp",
                        rapidjson::Value(timestamp.c_str(), allocator),
                        allocator);
    }

    void deserialize(rapidjson::Document &srcDoc) override
    {
      if (srcDoc.HasMember("networkStatus") && srcDoc["networkStatus"].IsInt())
      {
        networkStatus = static_cast<NetworkStatus>(srcDoc["networkStatus"].GetInt());
      }
      if (srcDoc.HasMember("timestamp") && srcDoc["timestamp"].IsString())
      {
        timestamp = srcDoc["timestamp"].GetString();
      }
    }

  public:
    NetworkStatus networkStatus = NetworkStatus::Online;
    std::string timestamp = "";
  };

  /**
   * @class NetworkEvent
   * Specialized event class for network status changes.
   */
  class NetworkEvent : public events_comm::TrEvent<NetworkEventType>
  {
  public:
    NetworkEvent(NetworkEventType type)
        : TrEvent<NetworkEventType>(type)
    {
    }
    NetworkEvent(NetworkEventType type, const NetworkEventDetail &detail)
        : TrEvent<NetworkEventType>(type)
    {
      detailStorage.setJsonFromInstance(const_cast<NetworkEventDetail &>(detail));
    }

    /**
     * Create a network event from a network status.
     */
    static std::shared_ptr<NetworkEvent> fromNetworkStatus(NetworkStatus status)
    {
      NetworkEventType eventType = (status == NetworkStatus::Online)
                                     ? NetworkEventType::Online
                                     : NetworkEventType::Offline;

      NetworkEventDetail detail(status);
      detail.timestamp = getCurrentTimestamp();

      return std::make_shared<NetworkEvent>(eventType, detail);
    }

  private:
    static std::string getCurrentTimestamp()
    {
      auto now = std::chrono::system_clock::now();
      auto time_t = std::chrono::system_clock::to_time_t(now);
      std::stringstream ss;
      ss << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S");
      return ss.str();
    }
  };

} // namespace runtime