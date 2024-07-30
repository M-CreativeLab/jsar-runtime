#pragma once

#include <string>
#include <vector>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "idgen.hpp"

using namespace std;

namespace events_comm
{
  static TrIdGenerator eventIdGenerator(1);

  /**
   * The Event class.
   */
  template <typename EventType, typename DetailType>
  class TrEvent
  {
  public:
    TrEvent(EventType type, std::optional<DetailType> detail = std::nullopt)
        : id(eventIdGenerator.get()), type(type), detail(detail)
    {
    }
    TrEvent(TrEvent &that)
        : id(that.id), type(that.type), detail(that.detail)
    {
    }

  public:
    int id;
    EventType type;
    optional<DetailType> detail;
  };

  class TrEventDetailBase
  {
  public:
    TrEventDetailBase() = default;
    TrEventDetailBase(string jsonSource) : jsonSource(jsonSource)
    {
    }
    ~TrEventDetailBase() = default;

  public:
    string &getString() { return jsonSource; }
    size_t size() { return jsonSource.size(); }

    template <typename T>
    T get()
    {
      rapidjson::Document jsonDoc;
      jsonDoc.Parse(jsonSource.c_str());
      return T(jsonDoc);
    }

  private:
    string jsonSource;
  };
}
