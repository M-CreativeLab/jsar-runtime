#ifdef TR_ENABLE_INSPECTOR

#include <iostream>

#include "./content_inspector.hpp"
#include "common/ipc.hpp"

using namespace std;

ContentInspector::ContentInspector()
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
  inspectorChanClient = new ipc::TrOneShotClient<inspector_comm::TrInspectorCommandMessage>(inspectorChanPort);
  if (!inspectorChanClient->connect())
  {
    delete inspectorChanClient;
    inspectorChanClient = nullptr;
    return false;
  }

  inspectorChanSender = make_unique<inspector_comm::TrInspectorCommandSender>(inspectorChanClient);
  inspectorChanReceiver = make_unique<inspector_comm::TrInspectorReceiver>(inspectorChanClient);

  // Create CDP handler with event sender callback
  auto eventSender = [this](const string &eventJson) -> bool
  {
    inspector_comm::TrCdpEvent event;
    event.cdpEventJson = eventJson;
    return inspectorChanSender->sendCommand(event);
  };

  contentCdpHandler = make_unique<ContentCdpHandler>(eventSender);

  return true;
}

void ContentInspector::start()
{
  if (!inspectorChanReceiver)
    return;

  // Start worker thread for handling inspector commands
  inspectorCommandWorker = make_unique<WorkerThread>([this]()
                                                     {
    inspector_comm::TrInspectorCommandMessage message;
    if (inspectorChanReceiver->recvCommand(message, 1000))
    {
      handleInspectorCommand(message);
    } });
}

void ContentInspector::stop()
{
  if (inspectorCommandWorker)
  {
    inspectorCommandWorker.reset();
  }

  if (inspectorChanClient)
  {
    delete inspectorChanClient;
    inspectorChanClient = nullptr;
  }
}

bool ContentInspector::sendInspectorResponse(inspector_comm::TrInspectorCommandBase &response)
{
  if (!inspectorChanSender)
    return false;
  return inspectorChanSender->sendCommand(response);
}

void ContentInspector::handleInspectorCommand(const inspector_comm::TrInspectorCommandMessage &message)
{
  if (!contentCdpHandler)
    return;

  switch (message.commandType)
  {
  case inspector_comm::TrInspectorCommandType::CdpRequest:
  {
    auto cdpRequest = inspector_comm::TrInspectorCommandBase::CreateFromMessage<inspector_comm::TrCdpRequest>(message);

    // Process the CDP message and get response
    string response = contentCdpHandler->processMessage(cdpRequest.cdpRequestJson);

    // Send response back to host
    inspector_comm::TrCdpResponse cdpResponse;
    cdpResponse.requestId = cdpRequest.requestId;
    cdpResponse.cdpResponseJson = response;
    inspectorChanSender->sendCommand(cdpResponse);
    break;
  }
  default:
    cerr << "ContentInspector: Unknown inspector command type: " << static_cast<int>(message.commandType) << endl;
    break;
  }
}

#endif // TR_ENABLE_INSPECTOR