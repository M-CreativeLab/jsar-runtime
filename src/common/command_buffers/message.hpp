#pragma once

#include "./shared.hpp"

namespace commandbuffers
{
  class TrCommandBufferSegment
  {
  public:
    TrCommandBufferSegment(size_t size, void *data) : size(size),
                                                      data(data)
    {
    }

    /**
     * It creates a segment from a vector<T> object.
     */
    template <typename T>
    TrCommandBufferSegment(vector<T> &vec)
    {
      size = vec.size() * sizeof(T);
      data = malloc(size);
      memcpy(data, vec.data(), size);
    }

  public:
    size_t getSize()
    {
      return size;
    }
    void *getData()
    {
      return data;
    }

    string toString()
    {
      return string((char *)data, size);
    }
    const char *c_str()
    {
      return (const char *)data;
    }

    /**
     * It reads the segment data as a vector<T> object.
     */
    template <typename T>
    vector<T> toVec()
    {
      vector<T> vec;
      vec.resize(size / sizeof(T));
      memcpy(vec.data(), data, size);
      return vec;
    }

  private:
    void dispose()
    {
      if (data != nullptr)
      {
        free(data);
        data = nullptr;
      }
      size = 0;
    }

  private:
    size_t size;
    void *data;

    friend class TrCommandBufferMessage;
  };

  class TrCommandBufferMessage
  {
  private:
    enum Usage
    {
      USAGE_NOTSET,
      USAGE_SERIALIZE,
      USAGE_DESERIALIZE,
    };

  public:
    TrCommandBufferMessage() : type(COMMAND_BUFFER_UNKNOWN), id(0), baseSize(0), base(nullptr) {}
    TrCommandBufferMessage(CommandBufferType type, size_t size, void *base) : type(type),
                                                                              id(commandBufferIdGen.get()),
                                                                              baseSize(size),
                                                                              base(base)
    {
    }
    ~TrCommandBufferMessage()
    {
      if (usage == USAGE_DESERIALIZE)
      {
        if (base != nullptr)
        {
          free(base);
          base = nullptr;
        }
        for (auto segment : segments)
          segment.dispose();
      }
    }

  public:
    void addSegment(TrCommandBufferSegment segment)
    {
      segments.push_back(segment);
    }
    void addStringSegment(string &str)
    {
      TrCommandBufferSegment segment(str.size() + 1, (void *)str.c_str());
      addSegment(segment);
    }
    TrCommandBufferSegment *getSegment(size_t index)
    {
      if (index >= segments.size())
        return nullptr;
      else
        return &segments[index];
    }
    size_t getSegmentCount()
    {
      return segments.size();
    }

    bool serialize(void **outData, size_t *outSize);
    bool deserialize(TrCommandBufferReceiver *receiver, int recvTimeout);
    bool deserialize(char *buffer, size_t size);
    bool deserializeContent(char *contentBuffer, size_t contentSize);

    template <typename T>
    T *createInstanceFromBase()
    {
      assert(base != nullptr);
      return new T(*(T *)base);
    }

  private:
    template <typename T>
    size_t readFrom(char *src, size_t offset, T *dest)
    {
      return readFrom(src, offset, dest, sizeof(T));
    }
    size_t readFrom(char *src, size_t offset, void *dest, size_t size)
    {
      memcpy(dest, src + offset, size);
      return offset + size;
    }

    template <typename T>
    size_t writeTo(char *dest, size_t offset, const T *src)
    {
      return writeTo(dest, offset, src, sizeof(T));
    }
    size_t writeTo(char *dest, size_t offset, const void *src, size_t size)
    {
      memcpy(dest + offset, src, size);
      return offset + size;
    }

  private:
    Usage usage = USAGE_NOTSET;
    CommandBufferType type;
    uint32_t id;
    size_t baseSize;
    void *base;
    vector<TrCommandBufferSegment> segments;

    friend class TrCommandBufferBase;
    friend class TrCommandBufferSender;
    friend class TrCommandBufferReceiver;
  };
}
