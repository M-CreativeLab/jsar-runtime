#ifdef TR_ENABLE_INSPECTOR

#include <iostream>

#include "./runtime_content_inspector.hpp"
#include "./cdp_handler.hpp"
#include "../constellation.hpp"
#include "common/ipc.hpp"

using namespace std;

RuntimeContentInspector::RuntimeContentInspector()
{
}

RuntimeContentInspector::~RuntimeContentInspector()
{
  // Don't delete the client - it's managed externally
}

bool RuntimeContentInspector::initialize(ipc::TrOneShotClient<inspector_comm::TrInspectorCommandMessage> *client)
{
  if (!client)
    return false;

  inspectorCommandChanClient = client;
  inspectorCommandChanSender = make_unique<inspector_comm::TrInspectorCommandSender>(client);
  inspectorCommandChanReceiver = make_unique<inspector_comm::TrInspectorReceiver>(client);

  return true;
}

bool RuntimeContentInspector::sendInspectorCommand(inspector_comm::TrInspectorCommandBase &command)
{
  if (!inspectorCommandChanSender)
    return false;
  return inspectorCommandChanSender->sendCommand(command);
}

void RuntimeContentInspector::handleInspectorCommandMessage(const inspector_comm::TrInspectorCommandMessage &message)
{
  switch (message.commandType)
  {
  case inspector_comm::TrInspectorCommandType::CdpResponse:
  {
    auto cdpResponse = inspector_comm::TrInspectorCommandBase::CreateFromMessage<inspector_comm::TrCdpResponse>(message);

    // Get CDP handler and forward response
    if (inspectorCdpHandler_)
    {
      auto contentProxy = inspectorCdpHandler_->getContentProxy();
      if (contentProxy)
      {
        contentProxy->handleResponseFromContent(cdpResponse.requestId, cdpResponse.cdpResponseJson);
      }
    }
    break;
  }
  case inspector_comm::TrInspectorCommandType::CdpEvent:
  {
    auto cdpEvent = inspector_comm::TrInspectorCommandBase::CreateFromMessage<inspector_comm::TrCdpEvent>(message);

    // Forward CDP events to inspector clients
    if (inspectorCdpHandler_)
    {
      auto constellation = inspectorCdpHandler_->getConstellation();
      if (constellation && constellation->inspector)
      {
        constellation->inspector->broadcastEventToClients(cdpEvent.cdpEventJson);
      }
    }
    break;
  }
  default:
    cerr << "RuntimeContentInspector: Unknown inspector command type: " << static_cast<int>(message.commandType) << endl;
    break;
  }
}

#endif // TR_ENABLE_INSPECTOR