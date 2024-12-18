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
  static TrIdGenerator eventIdGenerator(0x10);

  class TrEventDetailObject
  {
  public:
    TrEventDetailObject() = default;
    virtual ~TrEventDetailObject() = default;

  public:
    virtual void serialize(rapidjson::Document &destDoc) = 0;
    virtual void deserialize(rapidjson::Document &srcDoc) = 0;
  };

  /**
   * The `TrEventDetailStorage` class is used to store the detail of an event.
   */
  class TrEventDetailStorage
  {
  public:
    TrEventDetailStorage() = default;
    ~TrEventDetailStorage() = default;

  public:
    /**
     * Get the stored detail as a JSON string.
     *
     * @return The a new string of the detail JSON.
     */
    string toJson() { return srcJson; }

    /**
     * Get a reference to the stored detail JSON string.
     */
    string &getJson() { return srcJson; }

    /**
     * Get the byte length of the stored detail.
     */
    size_t getByteLength() { return srcJson.size(); }

    /**
     * Set the detail from a `TrEventDetailObject` instance.
     *
     * @param instance The instance of the detail object which inherited from `TrEventDetailObject`.
     */
    template <typename InstanceType>
    void setJsonFromInstance(InstanceType &instance)
    {
      rapidjson::Document jsonDoc;
      jsonDoc.SetObject();
      instance.serialize(jsonDoc);

      rapidjson::StringBuffer buffer;
      rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
      jsonDoc.Accept(writer);
      setJsonFromString(buffer.GetString());
    }

    /**
     * Set the detail from a JSON string.
     */
    void setJsonFromString(string json) { srcJson = json; }

    /**
     * Get the detail as a `TrEventDetailObject` instance.
     *
     * @return The new instance of the detail object which inherited from `TrEventDetailObject`.
     */
    template <typename ObjectType>
    ObjectType getInstance()
    {
      ObjectType instance;
      rapidjson::Document jsonDoc;
      jsonDoc.Parse(srcJson.c_str());
      if (!jsonDoc.HasParseError())
        instance.deserialize(jsonDoc);
      return instance;
    }

    // TODO: support detail object caching to avoid parsing JSON multiple times?

  private:
    string srcJson;
  };

  /**
   * The Event class.
   */
  template <typename EventType>
  class TrEvent
  {
  public:
    /**
     * Create a new event with the given type and detail.
     *
     * @tparam DetailObjectType The type of the detail object.
     * @tparam InstanceType The type of the event instance to create.
     * @param type The type of the event.
     * @param detail The detail of the event, it is optional.
     */
    template <typename InstanceType = TrEvent<EventType>, typename DetailObjectType>
    static inline std::shared_ptr<InstanceType> MakeEvent(EventType type, DetailObjectType *detail = nullptr)
    {
      auto event = std::make_shared<InstanceType>(type);
      if (detail != nullptr)
        event->detailStorage.setJsonFromInstance(*detail);
      return event;
    }

    /**
     * Create a new event with the given type and detail JSON string.
     *
     * @tparam InstanceType The type of the event instance to create.
     * @param type The type of the event.
     * @param detailJsonPtr A null-terminated pointer to the detail's JSON string.
     */
    template <typename InstanceType = TrEvent<EventType>>
    static inline std::shared_ptr<InstanceType> MakeEventWithString(EventType type, const char *detailJsonPtr)
    {
      auto event = std::make_shared<InstanceType>(type);
      if (detailJsonPtr != nullptr)
        event->detailStorage.setJsonFromString(string(detailJsonPtr));
      return event;
    }

  public:
    TrEvent(EventType type)
        : id(eventIdGenerator.get()), type(type)
    {
    }
    TrEvent(TrEvent &that)
        : id(that.id), type(that.type), detailStorage(that.detailStorage)
    {
    }
    virtual ~TrEvent() = default;

  public:
    /**
     * Get the detail as a `TrEventDetailObject` instance.
     *
     * @return The new instance of the detail object which inherited from `TrEventDetailObject`.
     */
    template <typename T>
    T detail()
    {
      return detailStorage.getInstance<T>();
    }

    /**
     * Get the detail as a JSON string.
     *
     * @return A new string of the detail JSON.
     */
    string &getDetailJson() { return detailStorage.getJson(); }

    /**
     * Get the byte length of the detail, it's useful when sometimes you just want to know the size of the detail.
     */
    size_t getDetailByteLength() { return detailStorage.getByteLength(); }

  public:
    int id;
    EventType type;

  protected:
    TrEventDetailStorage detailStorage;
  };
}
