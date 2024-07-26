#include "ipc.hpp"
#include "common/command_buffers/shared.hpp"
#include "common/frame_request/types.hpp"
#include "common/events/message.hpp"
#include "common/media/message.hpp"
#include "common/xr/message.hpp"

namespace ipc
{
  constexpr int MIN_INET_PORT = 1024;
  constexpr int MAX_INET_PORT = 65535;
  constexpr int MAX_ATTEMPTS = 30;
  constexpr int8_t HANDSHAKE_MAGIC[2] = {0x03, 0x07};

  bool createUnixSocketAddr(struct sockaddr_un *addr, string path)
  {
    addr->sun_family = AF_UNIX;

    size_t pathLen = path.length();
    size_t addrLen = sizeof(addr->sun_path);
    if (pathLen >= addrLen)
      return false;

    memset(addr->sun_path, 0, addrLen);
    memcpy(addr->sun_path, path.c_str(), pathLen);
    DEBUG(LOG_TAG_IPC, "The path \"%s\" is set.", addr->sun_path);
    return true;
  }

  bool createInetSocketAddr(int fd, struct sockaddr_in *addr)
  {
    for (int i = 0; i < MAX_ATTEMPTS; i++)
    {
      int port = MIN_INET_PORT + rand() % (MAX_INET_PORT - MIN_INET_PORT);
      struct sockaddr_in testAddr;
      testAddr.sin_family = AF_INET;
      testAddr.sin_port = htons(port);
      testAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
      if (::bind(fd, (struct sockaddr *)&testAddr, sizeof(testAddr)) == -1)
      {
        if (errno == EADDRINUSE)
          continue;
        else
        {
          DEBUG(LOG_TAG_IPC, "Failed to bind a socket(%d): %s", port, strerror(errno));
          return false;
        }
      }

      addr->sin_family = AF_INET;
      addr->sin_port = htons(port);
      addr->sin_addr.s_addr = INADDR_LOOPBACK;
      return true;
    }
    return false;
  }

  bool connectToSocket(int &fd, int port)
  {
    if (fd != -1)
      return false;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
      DEBUG(LOG_TAG_IPC, "Failed to create a socket: %s", strerror(errno));
      return false;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (::connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
      close(fd);
      fd = -1;
      DEBUG(LOG_TAG_IPC, "Failed to connect to the server: %s", strerror(errno));
      return false;
    }
    return true;
  }

  bool setNonBlockingOnSocket(int fd)
  {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
      DEBUG(LOG_TAG_IPC, "Failed to get the fd(%d) flags: %s", fd, strerror(errno));
      return false;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
      DEBUG(LOG_TAG_IPC, "Failed to set the fd(%d) to non-blocking mode: %s", fd, strerror(errno));
      return false;
    }
    return true;
  }

  template <typename T>
  TrChannelSender<T>::TrChannelSender(TrOneShotClient<T> *client) : client(client),
                                                                    fd(client->fd),
                                                                    blocking(client->blocking)
  {
  }

  template <typename T>
  TrChannelSender<T>::~TrChannelSender()
  {
    fd = -1;
    client = nullptr;
  }

  template <typename T>
  bool TrChannelSender<T>::send(T data)
  {
    return sendRaw(&data, sizeof(data));
  }

  template <typename T>
  bool TrChannelSender<T>::sendRaw(const void *data, size_t size)
  {
    if (fd == -1 || client == nullptr || client->invalid())
      return false;

    int bytesSent = 0;
    while (bytesSent < size)
    {
      size_t remaining = size - bytesSent;
      ssize_t sent = ::send(fd, (char *)data + bytesSent, remaining, 0);
      if (sent == -1)
      {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
          continue;
        if (errno == ECONNRESET || errno == EPIPE)
          client->invalid(true);
        DEBUG(LOG_TAG_IPC, "Failed to send data(bytes=%d): %s", remaining, strerror(errno));
        return false;
      }
      else
      {
        bytesSent += sent;
      }
    }
    assert(bytesSent == size); // The bytes to be sent is expected to be the specific `size`.
    return true;
  }

  template <typename T>
  TrChannelReceiver<T>::TrChannelReceiver(TrOneShotClient<T> *client) : client(client),
                                                                        fd(client->fd),
                                                                        blocking(client->blocking)
  {
  }

  template <typename T>
  TrChannelReceiver<T>::~TrChannelReceiver()
  {
    fd = -1;
    client = nullptr;
  }

  template <typename T>
  T *TrChannelReceiver<T>::tryRecv(int timeout)
  {
    T data;
    if (tryRecvRaw(&data, sizeof(T), timeout))
    {
      T *newData = new T(data);
      return newData;
    }
    else
    {
      return nullptr;
    }
  }

  template <typename T>
  bool TrChannelReceiver<T>::tryRecvRaw(void *outData, size_t outSize, int timeout)
  {
    if (client == nullptr || client->invalid())
      return false;

    if (blocking)
    {
      DEBUG(LOG_TAG_IPC, "The receiver is in blocking mode, thus tryRecv() is not available.");
      return false;
    }

    struct pollfd fds[1];
    fds[0].fd = fd;
    fds[0].events = POLLIN;

    int events = poll(fds, 1, timeout);
    if (events <= -1)
    {
      if (errno == EINTR)
        return this->tryRecvRaw(outData, outSize, timeout);

      DEBUG(LOG_TAG_IPC, "Failed to poll the receiver: %s", strerror(errno));
      return false;
    }

    if (fds[0].revents & POLLIN)
    {
      int bytesReceived = 0;
      while (bytesReceived < outSize)
      {
        size_t remaining = outSize - bytesReceived;
        auto received = ::recv(fd, (char *)outData + bytesReceived, remaining, 0);
        if (received <= 0 /** actuall 0 or -1 */)
        {
          string msg;
          if (received == 0)
          {
            msg = "The connection is closed by the peer.";
            client->invalid(true);
          }
          else if (errno == EAGAIN || errno == EWOULDBLOCK)
          {
            continue;
          }
          else
          {
            msg = strerror(errno);
          }
          DEBUG(LOG_TAG_IPC, "Failed to read data from socket(%d): %s", fd, msg.c_str());
          return false;
        }
        else
        {
          bytesReceived += received;
        }
      }
      assert(bytesReceived == outSize); // The bytes to be received is expected to be the specific `outSize`.
      return true;
    }
    else
    {
      return false;
    }
  }

  template <typename T>
  TrOneShotClient<T> *TrOneShotClient<T>::MakeAndConnect(int port, bool blocking)
  {
    TrOneShotClient<T> *client = new TrOneShotClient<T>();
    if (
        client->connect(port, blocking) &&
        client->sendHandshake()) // If the client is connected and the handshake is successful.
    {
      client->handshaked = true;
      return client;
    }
    else
    {
      delete client;
      return nullptr;
    }
  }

  template <typename T>
  TrOneShotClient<T>::TrOneShotClient() : connected(false)
  {
  }

  template <typename T>
  TrOneShotClient<T>::TrOneShotClient(int fd, bool blocking) : fd(fd), blocking(blocking), connected(true)
  {
    if (recvHandshake())
      handshaked = true;
    else
      disconnect();
  }

  template <typename T>
  TrOneShotClient<T>::~TrOneShotClient()
  {
    if (fd != -1)
    {
      ::close(fd);
      fd = -1;
    }
    connected = false;
  }

  template <typename T>
  bool TrOneShotClient<T>::connect(int port, bool blocking)
  {
    if (connected == true)
      return true;

    if (!connectToSocket(fd, port))
    {
      DEBUG(LOG_TAG_IPC, "Failed to connect to the server.");
      return false;
    }
    connected = true;

    if (blocking == false)
    {
      if (!setNonBlockingOnSocket(fd))
      {
        DEBUG(LOG_TAG_IPC, "Failed to set the socket to non-blocking mode, switching to blocking mode.");
        blocking = true;
      }
    }
    // Update the port and blocking mode.
    this->port = port;
    this->blocking = blocking;
    return true;
  }

  template <typename T>
  void TrOneShotClient<T>::disconnect()
  {
    if (fd != -1)
    {
      ::close(fd);
      fd = -1;
    }
    connected = false;
  }

  template <typename T>
  bool TrOneShotClient<T>::sendHandshake()
  {
    if (fd == -1 || connected == false)
      return false;

    /**
     * Create the handshake message [0x0f, 0x07, pid(4 bytes)]
     */
    char head[6];
    head[0] = HANDSHAKE_MAGIC[0]; /* magic number */
    head[1] = HANDSHAKE_MAGIC[1]; /* magic number */
    pid_t pid = getpid();
    memcpy(&head[2], &pid, sizeof(pid));

    /**
     * Send the handshake message.
     */
    ssize_t sent = ::send(fd, head, sizeof(head), 0);
    if (sent == -1 || sent != sizeof(head))
    {
      disconnect();
      return false;
    }
    else
    {
      pid = head[1];
      return true;
    }
  }

  template <typename T>
  bool TrOneShotClient<T>::recvHandshake()
  {
    if (fd == -1 || connected == false)
      return false;

    struct pollfd fds[1];
    fds[0].fd = fd;
    fds[0].events = POLLIN;

    int events = poll(fds, 1, 1000); // wait handshake for 1 second.
    if (events <= -1)
    {
      DEBUG(LOG_TAG_IPC, "Failed to poll the client at recv() handshake: %s", strerror(errno));
      disconnect();
      return false;
    }
    else if (!(fds[0].revents & POLLIN))
    {
      DEBUG(LOG_TAG_IPC, "Failed to poll the client at recv() handshake: The client is not ready or timeout(1s).");
      disconnect();
      return false;
    }

    char head[6];
    ssize_t received = ::recv(fd, head, sizeof(head), 0);
    if (received == -1 || received != sizeof(head))
    {
      disconnect();
      return false;
    }
    else
    {
      if (head[0] != HANDSHAKE_MAGIC[0] || head[1] != HANDSHAKE_MAGIC[1])
      {
        DEBUG(LOG_TAG_IPC, "The handshake is failed.");
        disconnect();
        return false;
      }
      // convert head[2] ~ head[5] to pid.
      memcpy(&pid, &head[2], sizeof(pid));
      return true;
    }
  }

  template <typename T>
  void TrOneShotClient<T>::invalid(bool flag)
  {
    invalidFlag = flag;
    if (flag == true)
      DEBUG(LOG_TAG_IPC, "The client(#%d, port=%d) is marked as invalid.", pid, port);
  }

  template <typename T>
  bool TrOneShotClient<T>::invalid()
  {
    return invalidFlag;
  }

  template <typename T>
  pid_t TrOneShotClient<T>::getPid()
  {
    return pid;
  }

  template <typename T>
  bool TrOneShotClient<T>::isConnected()
  {
    return connected;
  }

  template <typename T>
  TrOneShotServer<T>::TrOneShotServer(string name) : fd(-1)
  {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
      DEBUG(LOG_TAG_IPC, "Failed to create a socket: %s", strerror(errno));
      return;
    }

    if (setNonBlocking() == false)
    {
      DEBUG(LOG_TAG_IPC, "Failed to set the socket to non-blocking mode, switching to blocking mode.");
      blocking = true;
    }
    else
    {
      blocking = false;
    }

    struct sockaddr_in addr;
    if (!createInetSocketAddr(fd, &addr))
    {
      close(fd);
      DEBUG(LOG_TAG_IPC, "Failed to create a socket address and bind, there is no available port.");
      return;
    }
    if (::listen(fd, 16) == -1)
    {
      close(fd);
      DEBUG(LOG_TAG_IPC, "Failed to listen a socket: %s", strerror(errno));
      return;
    }
    port = ntohs(addr.sin_port);

    // If the server is in non-blocking mode, we need to set the file descriptor to the readFds.
    if (blocking == false)
    {
      fds[0].fd = fd;
      fds[0].events = POLLIN;
    }
    DEBUG(LOG_TAG_IPC, "The server(%s) is listening on 127.0.0.1:%d", name.c_str(), port);
  }

  template <typename T>
  TrOneShotServer<T>::~TrOneShotServer()
  {
    if (fd != -1)
      close(fd);
    port = -1;

    for (auto client : clients)
      delete client;
    clients.clear();
  }

  template <typename T>
  int TrOneShotServer<T>::getPort()
  {
    return port;
  }

  template <typename T>
  TrOneShotClient<T> *TrOneShotServer<T>::accept()
  {
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientFd = ::accept(fd, reinterpret_cast<struct sockaddr *>(&clientAddr), &addrLen);
    if (clientFd == -1)
      return nullptr;

    struct linger linger;
    linger.l_onoff = 1;
    linger.l_linger = 30;
    if (setsockopt(clientFd, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger)) <= -1)
      DEBUG(LOG_TAG_IPC, "Failed to set SO_LINGER: %s", strerror(errno));

    // Update the client fd to be non-blocking if the server is in non-blocking mode.
    if (!blocking)
    {
      if (!setNonBlockingOnSocket(clientFd))
        DEBUG(LOG_TAG_IPC, "Failed to set the client socket(%d) to non-blocking mode.", clientFd);
    }

    clients.push_back(new TrOneShotClient<T>(clientFd, blocking));
    return clients.back();
  }

  template <typename T>
  bool TrOneShotServer<T>::tryAccept(std::function<void(TrOneShotClient<T> &)> connCb, int timeout)
  {
    if (blocking)
    {
      DEBUG(LOG_TAG_IPC, "The server is in blocking mode, thus tryAccept() is not available.");
      return false;
    }

    int events = poll(fds, 1, timeout);
    if (events <= -1)
    {
      DEBUG(LOG_TAG_IPC, "Failed to poll the server: %s", strerror(errno));
      return false;
    }
    if (fds[0].revents & POLLIN)
    {
      fds[0].revents = 0;
      auto newClient = accept();
      if (newClient != nullptr)
      {
        connCb(*newClient);
        return true;
      }
      else
      {
        DEBUG(LOG_TAG_IPC, "Failed to accept a new client: %s", strerror(errno));
      }
    }
    return false;
  }

  template <typename T>
  vector<TrOneShotClient<T> *> &TrOneShotServer<T>::getClients()
  {
    return clients;
  }

  template <typename T>
  void TrOneShotServer<T>::removeClient(TrOneShotClient<T> *client)
  {
    if (client == nullptr)
      return;
    for (auto it = clients.begin(); it != clients.end(); it++)
    {
      if (*it == client)
      {
        delete *it;
        clients.erase(it);
        break;
      }
    }
  }

  template <typename T>
  bool TrOneShotServer<T>::setNonBlocking()
  {
    return setNonBlockingOnSocket(fd);
  }

#define SPECIALIZE_TEMPLATE(T)         \
  template class TrChannelSender<T>;   \
  template class TrChannelReceiver<T>; \
  template class TrOneShotClient<T>;   \
  template class TrOneShotServer<T>;

  SPECIALIZE_TEMPLATE(commandbuffers::TrCommandBufferMessage)
  SPECIALIZE_TEMPLATE(frame_request::TrFrameRequestMessage)
  SPECIALIZE_TEMPLATE(events::TrEventMessage)
  SPECIALIZE_TEMPLATE(media_comm::TrMediaCommandMessage)
  SPECIALIZE_TEMPLATE(xr::TrXRCommandMessage)
#undef SPECIALIZE_TEMPLATE
}
