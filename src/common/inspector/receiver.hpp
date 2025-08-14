#pragma once

#include "common/ipc.hpp"
#include "./message.hpp"

using namespace std;
using namespace ipc;

namespace inspector_comm
{
  typedef TrChannelReceiver<TrInspectorCommandMessage> TrInspectorReceiver;
}