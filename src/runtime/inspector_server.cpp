#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <common/debug.hpp>
#include "./inspector_server.hpp"

using namespace std;

TrInspectorServer::TrInspectorServer(std::shared_ptr<TrInspector> inspector)
    : fd_(-1)
    , inspector_(inspector)
{
  fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (fd_ == -1)
    throw runtime_error("Failed to create a socket");

  if (setNonBlocking() == false)
    throw runtime_error("Failed to set the socket to non-blocking mode");

  struct sockaddr_in addr;
  if (!initAndBindAddr(fd_, &addr))
  {
    close(fd_);
    string msg = "Failed to bind to " + to_string(port) + ": " + string(strerror(errno));
    throw runtime_error(msg);
  }
  if (::listen(fd_, 8) == -1)
  {
    close(fd_);
    throw runtime_error("Failed to listen a socket");
  }
  port = ntohs(addr.sin_port);

  DEBUG(LOG_TAG_INSPECTOR, "The inspector is listening at 0.0.0.0:%d", port);
}

TrInspectorServer::~TrInspectorServer()
{
  if (fd_ != -1)
    close(fd_);
  port = -1;
}

void TrInspectorServer::tryAccept()
{
  while (true)
  {
    auto client = accept();
    if (client == nullptr)
      break;

    int clientFd = client->fd_;
    clients_.push_back(move(client));
  }
}

void TrInspectorServer::tick()
{
  if (clients_.empty())
    return;

  // Remove the closed clients
  for (auto it = clients_.begin(); it != clients_.end();)
  {
    if ((*it)->shouldClose_)
      it = clients_.erase(it);
    else
      it++;
  }

  struct pollfd fds[clients_.size()];
  for (size_t i = 0; i < clients_.size(); i++)
  {
    if (clients_[i]->shouldClose_)
      continue;
    fds[i].fd = clients_[i]->fd_;
    fds[i].events = POLLIN;
  }

  int events = poll(fds, clients_.size(), 0);
  if (events <= -1)
  {
    DEBUG(LOG_TAG_ERROR, "Failed to poll the inspector server: %s", strerror(errno));
    return;
  }

  for (size_t i = 0; i < clients_.size(); i++)
  {
    if (fds[i].revents & POLLIN)
      clients_[i]->tick();
  }
}

bool TrInspectorServer::setNonBlocking()
{
  int flags = fcntl(fd_, F_GETFL, 0);
  if (flags == -1)
  {
    DEBUG(LOG_TAG_ERROR, "Failed to get the fd(%d) flags: %s", fd_, strerror(errno));
    return false;
  }
  if (fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1)
  {
    DEBUG(LOG_TAG_ERROR, "Failed to set the fd(%d) to non-blocking mode: %s", fd_, strerror(errno));
    return false;
  }
  return true;
}

bool TrInspectorServer::initAndBindAddr(int fd, struct sockaddr_in *addr)
{
  int opt = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    return false;

  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  addr->sin_addr.s_addr = htonl(INADDR_ANY);
  return ::bind(fd, (struct sockaddr *)addr, sizeof(*addr)) != -1;
}

unique_ptr<TrInspectorClient> TrInspectorServer::accept()
{
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  int clientFd = ::accept(fd_, (struct sockaddr *)&addr, &len);
  if (clientFd == -1)
  {
    if (errno != EAGAIN && errno != EWOULDBLOCK)
      DEBUG(LOG_TAG_ERROR, "Failed to accept a new client: %s", strerror(errno));
    return nullptr;
  }

  auto inspector = inspector_.lock();
  if (inspector == nullptr)
  {
    ::close(clientFd);
    return nullptr;
  }
  else
  {
    return make_unique<TrInspectorClient>(clientFd, inspector);
  }
}
