#pragma once

#include <vector>

#include "messages.hpp"
#include "ipc.hpp"
#include "idgen.hpp"

using namespace std;

class TrCommandBufferSender;
class TrCommandBufferReceiver;
class TrCommandBufferMessage;
class TrCommandBufferSubMessage;

enum CommandBufferType
{
  COMMAND_BUFFER_UNKNOWN = 0,
  COMMAND_BUFFER_WEBGL_CONTEXT_INIT_REQ,
  COMMAND_BUFFER_WEBGL_CONTEXT_INIT_RES,
  COMMAND_BUFFER_WEBGL2_CONTEXT_INIT_REQ,
  COMMAND_BUFFER_WEBGL2_CONTEXT_INIT_RES,
};

static TrIdGenerator commandBufferIdGen(1);

class TrCommandBufferSegment
{
public:
  TrCommandBufferSegment(size_t size, void *data) : size(size),
                                                    data(data)
  {
  }

public:
  string toString()
  {
    return string((char *)data, size);
  }
  const char *c_str()
  {
    return (const char *)data;
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

class TrCommandBufferBase
{
public:
  template <typename T>
  static T *CreateFromMessage(TrCommandBufferMessage &message)
  {
    T *commandBuffer = message.createInstanceFromBase<T>();
    commandBuffer->deserialize(message);
    return commandBuffer;
  }

public:
  TrCommandBufferBase() {}
  TrCommandBufferBase(CommandBufferType type) : type(type) {}

public:
  virtual TrCommandBufferMessage *serialize() = 0;
  virtual void deserialize(TrCommandBufferMessage &message) = 0;

public:
  CommandBufferType type = COMMAND_BUFFER_UNKNOWN;
  uint32_t id = commandBufferIdGen.get();
  size_t size = 0;
};

class TrCommandBufferResponse : public TrCommandBufferBase
{
public:
  TrCommandBufferResponse(CommandBufferType type, TrCommandBufferBase *req) : TrCommandBufferBase(type),
                                                                              requestId(req->id)
  {
  }

public:
  int requestId;
};

class WebGL1ContextInitCommandBufferRequest : public TrCommandBufferBase
{
public:
  WebGL1ContextInitCommandBufferRequest(int width, int height, string url) : TrCommandBufferBase(COMMAND_BUFFER_WEBGL_CONTEXT_INIT_REQ),
                                                                             width(width),
                                                                             height(height),
                                                                             url(url)
  {
    size = sizeof(WebGL1ContextInitCommandBufferRequest);
  }

public:
  TrCommandBufferMessage *serialize() override
  {
    auto message = new TrCommandBufferMessage(type, size, this);
    message->addStringSegment(url);
    return message;
  }
  void deserialize(TrCommandBufferMessage &message) override
  {
    auto urlSegment = message.getSegment(0);
    if (urlSegment != nullptr)
      url = urlSegment->toString();
  }

public:
  int width = 0;
  int height = 0;
  std::string url;
};

class WebGL1ContextInitCommandBufferResponse : public TrCommandBufferResponse
{
public:
  WebGL1ContextInitCommandBufferResponse(WebGL1ContextInitCommandBufferRequest *req)
      : TrCommandBufferResponse(COMMAND_BUFFER_WEBGL_CONTEXT_INIT_RES, req),
        foo(0)
  {
    size = sizeof(WebGL1ContextInitCommandBufferResponse);
  }

public:
  TrCommandBufferMessage *serialize() override
  {
    auto message = new TrCommandBufferMessage(type, size, this);
    message->addStringSegment(url);
    return message;
  }
  void deserialize(TrCommandBufferMessage &message) override
  {
    auto urlSegment = message.getSegment(0);
    if (urlSegment != nullptr)
      url = urlSegment->toString();
  }

public:
  int foo;
  std::string url;
};

class TrCommandBufferSender : public ipc::TrChannelSender<TrCommandBufferMessage>
{
public:
  TrCommandBufferSender(ipc::TrOneShotClient<TrCommandBufferMessage> *client) : ipc::TrChannelSender<TrCommandBufferMessage>(client) {}
  ~TrCommandBufferSender() {}

public:
  bool sendCommandBufferRequest(TrCommandBufferBase &req);
  bool sendCommandBufferResponse(TrCommandBufferResponse &res);
};

class TrCommandBufferReceiver : public ipc::TrChannelReceiver<TrCommandBufferMessage>
{
public:
  TrCommandBufferReceiver(ipc::TrOneShotClient<TrCommandBufferMessage> *client) : ipc::TrChannelReceiver<TrCommandBufferMessage>(client) {}
  ~TrCommandBufferReceiver() {}

public:
  TrCommandBufferBase *recvCommandBufferRequest(int timeout = 0);
  TrCommandBufferResponse *recvCommandBufferResponse(int timeout = 0);

  friend class TrCommandBufferMessage;
};
