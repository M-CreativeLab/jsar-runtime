#include <iostream>
#include <common/ipc.hpp>
#include <runtime/constellation.hpp>
#include <runtime/inspector.hpp>

#include "./cdp_handler.hpp"
#include "./content_domain_proxy.hpp"
#include "./content_inspector_proxy.hpp"

using namespace std;

ContentInspectorProxy::ContentInspectorProxy(shared_ptr<TrInspector> inspector)
    : inspector_(inspector)
{
}

ContentInspectorProxy::~ContentInspectorProxy()
{
  // Don't delete the client - it's managed externally
}

bool ContentInspectorProxy::initialize(ipc::TrOneShotClient<inspector_comm::TrInspectorCommandMessage> *client)
{
  if (!client)
    return false;

  inspectorCommandChanClient_ = client;
  inspectorCommandChanSender_ = make_unique<inspector_comm::TrInspectorCommandSender>(client);
  inspectorCommandChanReceiver_ = make_unique<inspector_comm::TrInspectorReceiver>(client);

  return true;
}

bool ContentInspectorProxy::sendInspectorCommand(inspector_comm::TrInspectorCommandBase &command)
{
  if (!inspectorCommandChanSender_)
    return false;
  return inspectorCommandChanSender_->sendCommand(command);
}

bool ContentInspectorProxy::recvInspectorCommand()
{
  inspector_comm::TrInspectorCommandMessage message;
  if (inspectorCommandChanReceiver_->recvCommand(message, 0))
  {
    handleInspectorCommandMessage(message);
    return true;
  }
  else
  {
    return false;
  }
}

void ContentInspectorProxy::handleInspectorCommandMessage(inspector_comm::TrInspectorCommandMessage &message)
{
  switch (message.getType())
  {
  case inspector_comm::TrInspectorCommandType::CdpResponse:
  {
    auto cdpResponse = inspector_comm::TrInspectorCommandBase::CreateFromMessage<inspector_comm::TrCdpResponse>(message);

    // Get CDP handler and forward response
    if (inspectorCdpHandler_)
    {
      auto contentProxy = inspectorCdpHandler_->getContentProxy();
      if (contentProxy)
        contentProxy->handleResponseFromContent(cdpResponse.requestId, cdpResponse.cdpResponseJson);
    }
    break;
  }
  case inspector_comm::TrInspectorCommandType::CdpEvent:
  {
    auto cdpEvent = inspector_comm::TrInspectorCommandBase::CreateFromMessage<inspector_comm::TrCdpEvent>(message);
    if (inspector_)
      inspector_->broadcastEventToClients(cdpEvent.cdpEventJson);
    break;
  }
  default:
    cerr << "ContentInspectorProxy: Unknown inspector command type: "
         << static_cast<int>(message.getType())
         << endl;
    break;
  }
}
