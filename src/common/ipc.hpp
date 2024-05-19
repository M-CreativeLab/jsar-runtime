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
#include <sys/poll.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef __ANDROID__
#include <sys/epoll.h>
#endif
#include "debug.hpp"
#include "messages.hpp"

using namespace std;

namespace ipc
{
  enum TrChannelType
  {
    TR_CHANNEL_TYPE_SHM,
    TR_CHANNEL_TYPE_PIPE,
    TR_CHANNEL_TYPE_SOCKET
  };

  template <typename T>
  class TrOneShotClient;

  template <typename T>
  class TrOneShotServer;

  template <typename T>
  class TrChannelSender
  {
  public:
    TrChannelSender(int port);
    TrChannelSender(TrOneShotClient<T> *client);
    ~TrChannelSender();

    bool connect();
    bool send(T data);

  private:
    int port;
    int fd;
    bool blocking;
  };

  template <typename T>
  class TrChannelReceiver
  {
  public:
    TrChannelReceiver(int fd);
    TrChannelReceiver(TrOneShotClient<T> *client);
    ~TrChannelReceiver();

  public:
    T *tryRecv(int timeout = 0);
    int getFd() { return fd; }

  private:
    int fd;
    bool blocking;
  };

  template <typename T>
  class TrOneShotClient
  {
  private:
    TrOneShotClient(int fd, bool blocking) : fd(fd), blocking(blocking)
    {
      DEBUG(LOG_TAG_IPC, "a new client is created(%d, blocking=%d)", fd, blocking);
    }
    int fd;
    bool blocking;

    friend class TrOneShotServer<T>;
    friend class TrChannelSender<T>;
    friend class TrChannelReceiver<T>;
  };

  /**
   * @brief A server that accepts only clients.
   */
  template <typename T>
  class TrOneShotServer
  {
  public:
    TrOneShotServer();
    ~TrOneShotServer();

    /**
     * Get the port number of this server.
     */
    int getPort();
    /**
     * Accepts a client in blocking mode.
     */
    TrOneShotClient<T> *accept();
    /**
     * Accepts a client in non-blocking mode.
     */
    TrOneShotClient<T> *tryAccept(int timeout = 0);
    /**
     * It returns the client list.
     */
    vector<TrOneShotClient<T> *> &getClients();

  private:
    bool setNonBlocking();

  private:
    int fd;
    int port;
    bool started = false;
    bool blocking = true;
    struct pollfd fds[1];
    vector<TrOneShotClient<T> *> clients;
  };
};
