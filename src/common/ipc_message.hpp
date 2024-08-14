#pragma once

#include <vector>
#include <string>
#include <assert.h>

#include "idgen.hpp"
#include "./ipc.hpp"

using namespace std;

namespace ipc
{
  constexpr int16_t TR_IPC_MESSAGE_MAGIC = 0x4D43; // 'CM'
  static TrIdGenerator ipcMessageIdGen(1);

  class TrIpcMessageSegment
  {
  public:
    TrIpcMessageSegment(size_t size, void *data, bool ownMemory = true)
        : size(size),
          ownMemory(ownMemory)
    {
      if (ownMemory)
      {
        this->data = new char[size];
        memcpy(this->data, data, size);
      }
      else
      {
        this->data = (char *)data;
      }
    }
    ~TrIpcMessageSegment()
    {
      if (ownMemory && data != nullptr)
        delete[] data;
    }

    /**
     * It creates a segment from a vector<T> object.
     */
    template <typename T>
    TrIpcMessageSegment(vector<T> &vec) : size(vec.size() * sizeof(T)), data(new char[size])
    {
      memcpy(this->data, vec.data(), size);
    }

  public:
    inline size_t getSize() { return size; }
    inline void *getData() { return (void *)data; }
    inline string toString() { return string(data, size); }
    inline const char *c_str() { return data; }

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

  public:
    size_t size;
    char *data;
    bool ownMemory = true;
  };

  template <typename MessageType, typename MessageEnum>
  class TrIpcMessage
  {
  private:
    enum Usage
    {
      USAGE_NOTSET,
      USAGE_SERIALIZE,
      USAGE_DESERIALIZE,
    };

  public:
    TrIpcMessage() : baseSize(0), base(nullptr) {}
    TrIpcMessage(MessageEnum type, size_t size, void *base)
        : type(type),
          baseSize(size),
          base(base)
    {
      id = ipcMessageIdGen.get();
    }
    ~TrIpcMessage()
    {
      if (usage == USAGE_DESERIALIZE)
      {
        if (base != nullptr)
        {
          free(base);
          base = nullptr;
        }
      }

      for (auto segment : segments)
        delete segment;
      segments.clear();
    }

  public:
    void addSegment(TrIpcMessageSegment *segment)
    {
      if (segment == nullptr)
        return;
      segments.push_back(segment);
    }
    void addRawSegment(size_t size, void *data)
    {
      addSegment(new TrIpcMessageSegment(size, data, false));
    }
    void addStringSegment(string &str)
    {
      addSegment(new TrIpcMessageSegment(str.size(), (void *)str.c_str()));
    }
    void addStringSegment(const char *str, size_t size)
    {
      addSegment(new TrIpcMessageSegment(size, (void *)str));
    }
    template <typename T>
    void addVecSegment(vector<T> &values)
    {
      addSegment(new TrIpcMessageSegment(values));
    }
    TrIpcMessageSegment *getSegment(size_t index)
    {
      if (index >= segments.size())
        return nullptr;
      else
        return segments[index];
    }
    size_t getSegmentCount()
    {
      return segments.size();
    }

    /**
     * It serializes this message, then allocates and copies the serialized buffer and its size into `outData` and `outSize`.
     *
     * @param outData a pointer to store the serialized buffer.
     * @param outSize the size of the `outData` when the serialization is finished.
     * @returns a boolean value if this operation is finished, false if allocation for the buffer failed.
     */
    bool serialize(void **outData, size_t *outSize)
    {
      usage = USAGE_SERIALIZE;

      size_t segmentsLength = 0;
      for (auto &segment : segments)
        segmentsLength += (sizeof(segment->size) + segment->size); // size + data
      size_t contentSize =
          sizeof(type) +     // type
          sizeof(uint32_t) + // id
          sizeof(size_t) +   // segments length
          sizeof(size_t) +   // segments count
          segmentsLength +   // segments
          sizeof(size_t) +   // base size
          baseSize;          // base

      int16_t magic = TR_IPC_MESSAGE_MAGIC;
      size_t bufferSize =
          sizeof(magic) +       // magic
          sizeof(contentSize) + // content size
          contentSize;          // content

      char *buffer = (char *)malloc(bufferSize);
      if (buffer == nullptr)
        return false; // out of memory

      size_t offset = 0;
      // Write header
      offset = writeTo(buffer, offset, &magic);
      offset = writeTo(buffer, offset, &contentSize, sizeof(contentSize));

      // Write metadata
      offset = writeTo(buffer, offset, &type, sizeof(type));
      offset = writeTo(buffer, offset, &id, sizeof(id));

      // Write segments
      size_t segmentsCount = segments.size();
      offset = writeTo(buffer, offset, &segmentsLength, sizeof(segmentsLength));
      offset = writeTo(buffer, offset, &segmentsCount, sizeof(size_t));
      for (auto &segment : segments)
      {
        offset = writeTo(buffer, offset, &segment->size, sizeof(segment->size));
        offset = writeTo(buffer, offset, segment->data, segment->size);
      }

      // Write base
      offset = writeTo(buffer, offset, &baseSize, sizeof(baseSize));
      offset = writeTo(buffer, offset, base, baseSize);
      assert(offset == bufferSize);

      *outData = buffer;
      *outSize = bufferSize;
      return true;
    }

#define STATIC_BUFFER_SIZE 1024 // 1KB

    bool deserialize(TrChannelReceiver<MessageType> *receiver, int recvTimeout)
    {
      usage = USAGE_DESERIALIZE; // mark as deserialized
      assert(base == nullptr);

      int16_t magic;
      if (!receiver->tryRecvRaw(&magic, sizeof(magic), recvTimeout))
        return false;
      if (magic != TR_IPC_MESSAGE_MAGIC)
        return false;

      size_t contentSize;
      if (!receiver->tryRecvRaw(&contentSize, sizeof(contentSize), recvTimeout))
        return false;

      /**
       * NOTE: Static buffer is an optimization to reduce the heap allocation, it avoids the heap allocation when the
       * content size is less than `STATIC_BUFFER_SIZE`, in most cases, the content size is less than 1024 bytes, thus
       * it could use the stack memory instead.
       */
      char staticBuffer[STATIC_BUFFER_SIZE];
      bool usingStaticBuffer = false;
      char *buffer = nullptr;
      if (contentSize > STATIC_BUFFER_SIZE)
      {
        buffer = (char *)malloc(contentSize);
        usingStaticBuffer = false;
      }
      else
      {
        buffer = staticBuffer;
        usingStaticBuffer = true;
      }

      if (!receiver->tryRecvRaw(buffer, contentSize, recvTimeout))
      {
        if (usingStaticBuffer == false && buffer != nullptr)
          free(buffer); // free the allocated buffer if failed to receive.
        return false;
      }

      int res = deserializeContent(buffer, contentSize);
      if (usingStaticBuffer == false && buffer != nullptr)
        free(buffer);
      return res;
    }

    bool deserialize(char *buffer, size_t size)
    {
      size_t offset = 0;
      int16_t magic;
      size_t contentSize;

      offset = readFrom(buffer, offset, &magic);
      offset = readFrom(buffer, offset, &contentSize);

      if (magic != TR_IPC_MESSAGE_MAGIC)
        return false;
      return deserializeContent(buffer + offset, contentSize);
    }

    bool deserializeContent(char *contentBuffer, size_t contentSize)
    {
      size_t offset = 0;
      offset = readFrom(contentBuffer, offset, &type);
      offset = readFrom(contentBuffer, offset, &id);

      size_t segmentsLength, segmentsCount;
      offset = readFrom(contentBuffer, offset, &segmentsLength);
      offset = readFrom(contentBuffer, offset, &segmentsCount);

      for (size_t i = 0; i < segmentsCount; i++)
      {
        size_t cSize;
        offset = readFrom(contentBuffer, offset, &cSize);
        void *cData = malloc(cSize);
        offset = readFrom(contentBuffer, offset, cData, cSize);
        if (cData != nullptr)
        {
          segments.push_back(new TrIpcMessageSegment(cSize, cData));
          free(cData);
        }
      }
      offset = readFrom(contentBuffer, offset, &baseSize);
      base = malloc(baseSize);
      offset = readFrom(contentBuffer, offset, base, baseSize);

      assert(offset == contentSize); // check if all content is read
      return true;
    }

    /**
     * It creates an instance of T from the base data.
     *
     * @return a new instance of T.
     */
    template <typename T>
    T *createInstanceFromBase()
    {
      assert(base != nullptr);
      T *baseRef = reinterpret_cast<T *>(base);
      return new T(*baseRef);
    }

    /**
     * It gets the reference to a T object which is stored in the `base` field, this is useful when the message's lifetime
     * is managed at stack, this could avoid the unnecessary heap allocation than `createInstanceFromBase`.
     *
     * NOTE:
     */
    template <typename T>
    T &getReferenceFromBase()
    {
      assert(base != nullptr);
      T *pBase = reinterpret_cast<T *>(base);
      return *pBase;
    }

  private:
    template <typename T>
    size_t readFrom(char *src, size_t offset, T *dest)
    {
      return readFrom(src, offset, dest, sizeof(T));
    }
    size_t readFrom(char *src, size_t offset, void *dest, size_t size)
    {
      if (dest != nullptr)
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

  protected:
    MessageEnum type;
    uint32_t id;
    vector<TrIpcMessageSegment *> segments;

  private:
    Usage usage = USAGE_NOTSET;
    size_t baseSize;
    void *base;
  };
}
