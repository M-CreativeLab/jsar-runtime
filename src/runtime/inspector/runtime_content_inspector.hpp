#pragma once

#ifdef TR_ENABLE_INSPECTOR

#include <memory>
#include "common/inspector/receiver.hpp"
#include "common/inspector/sender.hpp"
#include "common/inspector/message.hpp"

// Forward declarations
class CdpHandler;

// Encapsulates all inspector-related functionality for runtime content processes
class RuntimeContentInspector
{
public:
  RuntimeContentInspector();
  ~RuntimeContentInspector();

  // Initialize inspector channels
  bool initialize(ipc::TrOneShotClient<inspector_comm::TrInspectorCommandMessage> *client);

  // Set the CDP handler reference
  void setCdpHandler(CdpHandler *cdpHandler)
  {
    inspectorCdpHandler_ = cdpHandler;
  }

  // Get the CDP handler reference
  CdpHandler *getCdpHandler() const
  {
    return inspectorCdpHandler_;
  }

  // Send inspector command
  bool sendInspectorCommand(inspector_comm::TrInspectorCommandBase &command);

  // Handle incoming inspector command message
  void handleInspectorCommandMessage(const inspector_comm::TrInspectorCommandMessage &message);

private:
  // IPC channels for inspector communication
  ipc::TrOneShotClient<inspector_comm::TrInspectorCommandMessage> *inspectorCommandChanClient = nullptr;
  std::unique_ptr<inspector_comm::TrInspectorReceiver> inspectorCommandChanReceiver = nullptr;
  std::unique_ptr<inspector_comm::TrInspectorCommandSender> inspectorCommandChanSender = nullptr;

  // Reference to the inspector client (CdpHandler) that manages this content runtime
  CdpHandler *inspectorCdpHandler_ = nullptr;
};

#endif // TR_ENABLE_INSPECTOR