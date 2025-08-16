#pragma once

#include <memory>
#include "common/inspector/receiver.hpp"
#include "common/inspector/sender.hpp"
#include "common/inspector/message.hpp"
#include "common/scoped_thread.hpp"
#include "client/inspector/content_cdp_handler.hpp"

namespace client_inspector
{
  class ContentInspector
  {
  public:
    ContentInspector(uint32_t contentId);
    ~ContentInspector();

    // Initialize inspector channels and handlers
    bool initialize(int inspectorChanPort);

    // Start the inspector worker thread
    void start();

    // Stop the inspector worker thread
    void stop();

    // Get the ContentCdpHandler instance
    ContentCdpHandler *getContentCdpHandler() const
    {
      return contentCdpHandler.get();
    }

    // Send inspector response
    bool sendInspectorResponse(inspector_comm::TrInspectorCommandBase &response);

    // Handle incoming inspector command
    void handleInspectorCommand(inspector_comm::TrInspectorCommandMessage &message);

  private:
    uint32_t contentId_ = 0;

    // IPC channels for inspector communication
    ipc::TrOneShotClient<inspector_comm::TrInspectorCommandMessage> *inspectorChanClient = nullptr;
    std::unique_ptr<inspector_comm::TrInspectorCommandSender> inspectorChanSender = nullptr;
    std::unique_ptr<inspector_comm::TrInspectorReceiver> inspectorChanReceiver = nullptr;

    // Worker thread for handling inspector commands
    std::unique_ptr<WorkerThread> inspectorCommandWorker = nullptr;

    // CDP handler for processing inspector commands
    std::unique_ptr<ContentCdpHandler> contentCdpHandler = nullptr;
  };
}