#include "ipc.hpp"

namespace ipc
{
  constexpr int MIN_INET_PORT = 1024;
  constexpr int MAX_INET_PORT = 65535;
  constexpr int MAX_ATTEMPTS = 30;

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

  template <typename T>
  TrChannelSender<T>::TrChannelSender(int port) : port(port), fd(-1)
  {
  }

  template <typename T>
  TrChannelSender<T>::~TrChannelSender()
  {
    if (fd != -1)
      close(fd);
  }

  template <typename T>
  bool TrChannelSender<T>::connect()
  {
    if (fd != -1)
      return false;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
      DEBUG(LOG_TAG_IPC, "Failed to create a socket.");
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

  template <typename T>
  bool TrChannelSender<T>::send(T data)
  {
    if (fd == -1)
      return false;

    ssize_t sent = ::send(fd, &data, sizeof(data), 0);
    if (sent == -1)
    {
      DEBUG(LOG_TAG_IPC, "Failed to send data: %s", strerror(errno));
      return false;
    }
    return true;
  }

  template <typename T>
  TrChannelReceiver<T>::TrChannelReceiver(int fd) : fd(fd)
  {
    DEBUG(LOG_TAG_IPC, "The receiver is created with fd=%d", fd);
  }

  template <typename T>
  TrChannelReceiver<T>::~TrChannelReceiver()
  {
    if (fd != -1)
      close(fd);
  }

  template <typename T>
  T *TrChannelReceiver<T>::tryRecv()
  {
    if (fd == -1)
      return nullptr;

    T *data = new T();
    ssize_t bytesReceived = ::recv(fd, data, sizeof(T), 0);
    if (bytesReceived <= -1)
    {
      delete data;
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        return nullptr;

      DEBUG(LOG_TAG_IPC, "Failed to receive data: %s", strerror(errno));
      return nullptr;
    }
    return data;
  }

  template <typename T>
  TrOneShotServer<T>::TrOneShotServer() : fd(-1)
  {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
      DEBUG(LOG_TAG_IPC, "Failed to create a socket.");
      return;
    }

    struct sockaddr_in addr;
    if (!createInetSocketAddr(fd, &addr))
    {
      close(fd);
      DEBUG(LOG_TAG_IPC, "Failed to create a socket address and bind, there is no available port.");
      return;
    }
    if (::listen(fd, 1) == -1)
    {
      close(fd);
      DEBUG(LOG_TAG_IPC, "Failed to listen a socket: %s", strerror(errno));
      return;
    }
    port = ntohs(addr.sin_port);
    DEBUG(LOG_TAG_IPC, "The server is listening on 127.0.0.1:%d", port);
  }

  template <typename T>
  TrOneShotServer<T>::~TrOneShotServer()
  {
    if (fd != -1)
      close(fd);
    port = -1;

    for (auto receiver : receivers)
      delete receiver;
    receivers.clear();
  }

  template <typename T>
  int TrOneShotServer<T>::getPort()
  {
    return port;
  }

  template <typename T>
  TrChannelReceiver<T> *TrOneShotServer<T>::accept()
  {
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientFd = ::accept(fd, reinterpret_cast<struct sockaddr *>(&clientAddr), &addrLen);
    if (clientFd <= -1)
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        return nullptr;

      DEBUG(LOG_TAG_IPC, "Failed to accept a client: %s", strerror(errno));
      return nullptr;
    }

    struct linger linger;
    linger.l_onoff = 1;
    linger.l_linger = 30;
    if (setsockopt(clientFd, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger)) <= -1)
      DEBUG(LOG_TAG_IPC, "Failed to set SO_LINGER: %s", strerror(errno));

    receivers.push_back(new TrChannelReceiver<T>(clientFd));
    return receivers.back();
  }

  template <typename T>
  vector<TrChannelReceiver<T> *> &TrOneShotServer<T>::getReceivers()
  {
    return receivers;
  }

  template <typename T>
  bool TrOneShotServer<T>::setNonBlocking()
  {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
      return false;
    else if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
      return false;
    return true;
  }

#define SPECIALIZE_TEMPLATE(T)         \
  template class TrChannelSender<T>;   \
  template class TrChannelReceiver<T>; \
  template class TrOneShotServer<T>;

  SPECIALIZE_TEMPLATE(CustomEvent)
#undef SPECIALIZE_TEMPLATE
}
