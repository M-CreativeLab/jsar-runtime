#pragma once

#include <stdlib.h>
#include "idgen.hpp"
#include "./types.hpp"

namespace xr
{
  static const int16_t TR_XRCOMMAND_MAGIC = 0x5A5A;
  static TrIdGenerator xrCmdIdGen(1);

  enum class TrXRCmdType
  {
    /**
     * Request to check if the given session mode is supported.
     */
    IsSessionSupportedRequest,
    /**
     * Response to the `IsSessionSupportedRequest`.
     */
    IsSessionSupportedResponse,
    /**
     * Request to start a new session.
     */
    SessionRequest,
    /**
     * Response to the `SessionRequest`.
     */
    SessionResponse,
    /**
     * Request to end the current session.
     */
    EndSessionRequest,
    /**
     * Response to the `EndSessionRequest`.
     */
    EndSessionResponse,
    /**
     * Request to update the base layer.
     */
    UpdateBaseLayerRequest,
    /**
     * Response to the `UpdateBaseLayerRequest`.
     */
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
    int stereoId = 0;
    long long stereoTimestamp = 0;
    long long sentAtTimestamp = 0;
  };

  class IsSessionSupportedRequest : public TrXRCommandBase<IsSessionSupportedRequest>
  {
  public:
    IsSessionSupportedRequest(TrXRSessionMode mode) : TrXRCommandBase(TrXRCmdType::IsSessionSupportedRequest),
                                                      sessionMode(mode)
    {
    }

  public:
    TrXRSessionMode sessionMode;
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
    SessionRequest() : TrXRCommandBase(TrXRCmdType::SessionRequest),
                       sessionMode(TrXRSessionMode::ImmersiveAR)
    {
    }
    SessionRequest(TrXRSessionMode mode) : TrXRCommandBase(TrXRCmdType::SessionRequest),
                                           sessionMode(mode)
    {
    }

  public:
    TrXRSessionMode sessionMode;
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
    float recommendedContentSize = 1.0f;
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
    /**
     * It creates a new `TrXRCommandMessage` instance from the given `XRCommandBase` instance, the caller should
     * free the instance after use.
     */
    template <typename XRCommandType>
    static TrXRCommandMessage *Make(TrXRCommandBase<XRCommandType> &xrCommand)
    {
      return new TrXRCommandMessage(xrCommand.type, &xrCommand, xrCommand.baseSize);
    }
    /**
     * It creates a new `TrXRCommandMessage` instance from the given raw data, the caller should free the instance
     * after use.
     */
    static TrXRCommandMessage *MakeFromRaw(void *data, size_t size)
    {
      if (size < sizeof(TrXRCmdType))
        return nullptr;

      TrXRCmdType type = *(TrXRCmdType *)data;
      return new TrXRCommandMessage(type, (char *)data + sizeof(TrXRCmdType), size - sizeof(TrXRCmdType));
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
    ~TrXRCommandMessage()
    {
      if (base != nullptr)
      {
        free(base);
        base = nullptr;
      }
    }

  public:
    /**
     * It creates a new `XRCommandBase` instance from the current message, the caller should free the instance after use.
     */
    template <typename T>
    T *createInstance()
    {
      if (base == nullptr)
        return nullptr;
      assert(baseSize == sizeof(T));
      return new T(*reinterpret_cast<T *>(base));
    }

  public:
    TrXRCmdType type;
    void *base;
    size_t baseSize;
  };
}
