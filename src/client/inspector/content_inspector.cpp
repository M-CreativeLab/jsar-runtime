#include <iostream>
#include <memory>
#include <common/ipc.hpp>

#include "./content_inspector.hpp"

namespace client_inspector
{
  using namespace std;

  ContentInspector::ContentInspector(uint32_t contentId)
      : contentId_(contentId)
  {
  }

  ContentInspector::~ContentInspector()
  {
    stop();
  }

  bool ContentInspector::initialize(int inspectorChanPort)
  {
    if (inspectorChanPort == 0)
      return false;

    // Initialize inspector IPC channels
    inspectorChanClient_ = ipc::TrOneShotClient<inspector_comm::TrInspectorCommandMessage>::MakeAndConnect(inspectorChanPort, false, contentId_);
    if (!inspectorChanClient_ || !inspectorChanClient_->isConnected())
    {
      cerr << "ContentInspector: Failed to connect to inspector channel on port " << inspectorChanPort << endl;
      return false;
    }

    inspectorChanSender_ = make_unique<inspector_comm::TrInspectorCommandSender>(inspectorChanClient_);
    inspectorChanReceiver_ = make_unique<inspector_comm::TrInspectorReceiver>(inspectorChanClient_);

    // Create event sender function for CDP domains
    auto eventSender = [this](const string &eventJson) -> bool
    {
      inspector_comm::TrCdpEvent cdpEvent(this->contentId_, eventJson);
      bool result = sendInspectorResponse(cdpEvent);
      if (!result)
        DEBUG(LOG_TAG_CLIENT_ENTRY, "ClientContext(%d) failed to send CDP event", this->contentId_);
      return result;
    };

    // Initialize content-side CDP handler with event sender
    contentCdpHandler_ = make_unique<ContentCdpHandler>(eventSender);
    return true;
  }

  void ContentInspector::start()
  {
    assert(inspectorChanReceiver_ != nullptr && inspectorChanSender_ != nullptr &&
           "ContentInspector: Inspector channels are not initialized");

    // Create worker thread for processing inspector commands
    auto onInspectorCommandWork = [this](WorkerThread &worker)
    {
      inspector_comm::TrInspectorCommandMessage incomingCommand;
      if (inspectorChanReceiver_->recvCommand(incomingCommand, 100))
        handleInspectorCommand(incomingCommand);
    };
    inspectorCommandWorker_ = make_unique<WorkerThread>("TrInspectorCommandWorker", onInspectorCommandWork);
  }

  void ContentInspector::stop()
  {
    if (inspectorCommandWorker_)
    {
      inspectorCommandWorker_.reset();
    }

    if (inspectorChanClient_)
    {
      inspectorChanClient_->disconnect();
      inspectorChanClient_ = nullptr;
      // TODO(yorkie): delete the client?
    }
  }

  bool ContentInspector::sendInspectorResponse(inspector_comm::TrInspectorCommandBase &response)
  {
    if (!inspectorChanSender_)
      return false;
    return inspectorChanSender_->sendCommand(response);
  }

  void ContentInspector::handleInspectorCommand(inspector_comm::TrInspectorCommandMessage &message)
  {
    if (!contentCdpHandler_)
      return;

    auto messageType = message.getType();
    switch (messageType)
    {
    case inspector_comm::TrInspectorCommandType::CdpRequest:
    {
      auto cdpRequest = inspector_comm::TrInspectorCommandBase::CreateFromMessage<inspector_comm::TrCdpRequest>(message);

      // Use the ContentCdpHandler to process the request and get domain/method
      string responseJson;
      if (contentCdpHandler_)
      {
        responseJson = contentCdpHandler_->processMessage(cdpRequest.cdpMessageJson);
      }
      else
      {
        // Fallback error response if handler is not available
        responseJson = "{\"id\":" +
                       to_string(cdpRequest.requestId) +
                       ",\"error\":{\"code\":-32603,\"message\":\"Internal error: CDP handler not initialized\"}}";
      }

      // Send response back to host process
      inspector_comm::TrCdpResponse cdpResponse(cdpRequest.requestId, cdpRequest.contentId, responseJson);

      if (!sendInspectorResponse(cdpResponse))
        cerr << "ClientContext(" << contentId_ << ") failed to send CDP response " << cdpRequest.requestId << endl;

      break;
    }
    default:
      cerr << "ContentInspector: Unknown inspector command type: " << static_cast<int>(messageType) << endl;
      break;
    }
  }
}
