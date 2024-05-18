#pragma once

#include <vector>
#include <cstring>
#include <thread>
#include <iostream>
#include <csignal>
#include <functional>

#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef __ANDROID__
#include <sys/epoll.h>
#endif
#include "debug.hpp"

using namespace std;

namespace ipc
{
  class CustomEvent
  {
  public:
    CustomEvent() : foobar(0) {}
    CustomEvent(int v) : foobar(v) {}
    int foobar;
  };

  enum TrChannelType
  {
    TR_CHANNEL_TYPE_SHM,
    TR_CHANNEL_TYPE_PIPE,
    TR_CHANNEL_TYPE_SOCKET
  };

  template <typename T>
  class TrChannelSender
  {
  public:
    TrChannelSender(int port);
    ~TrChannelSender();

    bool connect();
    bool send(T data);

  private:
    int port;
    int fd;
  };

  template <typename T>
  class TrChannelReceiver
  {
  public:
    TrChannelReceiver(int fd);
    ~TrChannelReceiver();

  public:
    T *tryRecv();

  private:
    int fd;
  };

  template <typename T>
  class TrOneShotServer
  {
  public:
    TrOneShotServer();
    ~TrOneShotServer();

    int getPort();
    TrChannelReceiver<T> *accept();
    vector<TrChannelReceiver<T>*> &getReceivers();

  private:
    bool setNonBlocking();

  private:
    int fd;
    int port;
    bool started = false;
    vector<TrChannelReceiver<T>*> receivers;
  };
};
