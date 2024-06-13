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
    TrChannelSender(TrOneShotClient<T> *client);
    ~TrChannelSender();

  public:
    bool send(T data);
    bool sendRaw(const void *data, size_t size);

  private:
    int fd;
    bool blocking;
    TrOneShotClient<T> *client;
  };

  template <typename T>
  class TrChannelReceiver
  {
  public:
    TrChannelReceiver(TrOneShotClient<T> *client);
    ~TrChannelReceiver();

  public:
    T *tryRecv(int timeout = 0);
    int getFd() { return fd; }
    bool tryRecvRaw(void *outData, size_t outSize, int timeout);

  private:
    int fd;
    bool blocking;
    TrOneShotClient<T> *client;
  };

  /**
   * @brief A client that connects to a server.
   */
  template <typename T>
  class TrOneShotClient
  {
  public:
    static TrOneShotClient<T> *MakeAndConnect(int port, bool blocking);

  private:
    TrOneShotClient();
    TrOneShotClient(int fd, bool blocking);
    ~TrOneShotClient();

  private:
    bool connect(int port, bool blocking);
    void disconnect();
    bool sendHandshake();
    bool recvHandshake();
    void invalid(bool flag);
    bool invalid();

  public:
    pid_t getPid();
    bool isConnected();

  private:
    int fd = -1;
    int port = -1;
    pid_t pid = -1;
    bool blocking = true;
    bool connected = false;
    bool handshaked = false;
    bool invalidFlag = false;

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
    TrOneShotServer(string name);
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
    /**
     * It removes the client from the client list.
     */
    void removeClient(TrOneShotClient<T> *client);

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
