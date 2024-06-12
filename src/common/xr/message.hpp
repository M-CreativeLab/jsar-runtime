#pragma once

#include <stdlib.h>
#include "idgen.hpp"
#include "./types.hpp"

namespace xr
{
  static TrIdGenerator xrCmdIdGen(1);
  enum class TrXRCmdType
  {
    IsSessionSupportedRequest,
    IsSessionSupportedResponse,
    SessionRequest,
    SessionResponse,
    EndSessionRequest,
    EndSessionResponse,
    UpdateBaseLayerRequest,
    UpdateBaseLayerResponse,
    Unknown = -1,
  };

  template <typename T>
  class TrXRCommandBase
  {
  public:
    TrXRCommandBase() {}
    TrXRCommandBase(TrXRCmdType type) : type(type)
    {
    }
    virtual ~TrXRCommandBase() {}

  public:
    TrXRCmdType type;
    uint32_t id = xrCmdIdGen.get();
    size_t baseSize = sizeof(T);
  };

  class IsSessionSupportedRequest : public TrXRCommandBase<IsSessionSupportedRequest>
  {
  public:
    IsSessionSupportedRequest() : TrXRCommandBase(TrXRCmdType::IsSessionSupportedRequest) {}

  public:
    TrSessionMode sessionMode;
  };

  class IsSessionSupportedResponse : public TrXRCommandBase<IsSessionSupportedResponse>
  {
  public:
    IsSessionSupportedResponse() : TrXRCommandBase(TrXRCmdType::IsSessionSupportedResponse) {}

  public:
    bool supported;
  };

  class SessionRequest : public TrXRCommandBase<SessionRequest>
  {
  public:
    SessionRequest() : TrXRCommandBase(TrXRCmdType::SessionRequest) {}

  public:
    TrSessionMode sessionMode;
  };

  class SessionResponse : public TrXRCommandBase<SessionResponse>
  {
  public:
    SessionResponse(uint32_t sessionId) : TrXRCommandBase(TrXRCmdType::SessionResponse),
                                          sessionId(sessionId)
    {
    }

  public:
    bool success;
    uint32_t sessionId;
  };

  class EndSessionRequest : public TrXRCommandBase<EndSessionRequest>
  {
  public:
    EndSessionRequest(uint32_t sessionId) : TrXRCommandBase(TrXRCmdType::EndSessionRequest),
                                            sessionId(sessionId)
    {
    }

  public:
    uint32_t sessionId;
  };

  class EndSessionResponse : public TrXRCommandBase<EndSessionResponse>
  {
  public:
    EndSessionResponse() : TrXRCommandBase(TrXRCmdType::EndSessionResponse) {}

  public:
    bool success;
  };

  class UpdateBaseLayerRequest : public TrXRCommandBase<UpdateBaseLayerRequest>
  {
  public:
    UpdateBaseLayerRequest(uint32_t sessionId) : TrXRCommandBase(TrXRCmdType::UpdateBaseLayerRequest),
                                                 sessionId(sessionId)
    {
    }

  public:
    uint32_t sessionId;
    uint32_t baseLayerId;
    float depthFar;
    float depthNear;
    float inlineVerticalFieldOfView;
    // TODO: layers?
  };

  class UpdateBaseLayerResponse : public TrXRCommandBase<UpdateBaseLayerResponse>
  {
  public:
    UpdateBaseLayerResponse() : TrXRCommandBase(TrXRCmdType::UpdateBaseLayerResponse) {}

  public:
    bool success;
  };

  class TrXRCommandMessage
  {
  public:
    template <typename XRCommandType>
    static TrXRCommandMessage *Make(TrXRCommandBase<XRCommandType> &xrCommand)
    {
      return new TrXRCommandMessage(xrCommand.type, &xrCommand, xrCommand.baseSize);
    }

  public:
    TrXRCommandMessage() : type(TrXRCmdType::Unknown), base(nullptr), baseSize(0) {}
    TrXRCommandMessage(TrXRCmdType type, void *base, size_t baseSize)
        : type(type),
          baseSize(baseSize)
    {
      if (base == nullptr || baseSize == 0)
        return;

      this->base = malloc(baseSize);
      memcpy(this->base, base, baseSize);
    }

  public:
    ~TrXRCommandMessage()
    {
      if (base != nullptr)
      {
        free(base);
        base = nullptr;
      }
    }

  public:
    TrXRCmdType type;
    void *base;
    size_t baseSize;
  };
}
