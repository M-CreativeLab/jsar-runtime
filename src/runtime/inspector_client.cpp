#include <stdexcept>
#include <span>
#include <common/debug.hpp>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>

#include "./inspector_client.hpp"
#include "./inspector.hpp"

using namespace std;

TrInspectorClient::TrInspectorClient(int fd, shared_ptr<TrInspector> inspector)
    : fd_(fd), inspector_(inspector)
{
  if (setNonBlocking() == false)
    throw runtime_error("Failed to set the socket to non-blocking mode");

#define _INSTANCE_CALL(method, ...)                                      \
  if (parser != nullptr)                                                 \
    static_cast<TrInspectorClient *>(parser->data)->method(__VA_ARGS__); \
  return 0;

  parsingSettings_.on_url = [](llhttp_t *parser, const char *at, size_t length) -> int
  { _INSTANCE_CALL(onUrl, at, length); };
  parsingSettings_.on_header_field = [](llhttp_t *parser, const char *at, size_t length) -> int
  { _INSTANCE_CALL(onHeaderField, at, length); };
  parsingSettings_.on_header_field_complete = [](llhttp_t *parser) -> int
  { _INSTANCE_CALL(onHeaderFieldComplete); };
  parsingSettings_.on_header_value = [](llhttp_t *parser, const char *at, size_t length) -> int
  { _INSTANCE_CALL(onHeaderValue, at, length); };
  parsingSettings_.on_header_value_complete = [](llhttp_t *parser) -> int
  { _INSTANCE_CALL(onHeaderValueComplete); };
  parsingSettings_.on_message_complete = [](llhttp_t *parser) -> int
  { _INSTANCE_CALL(onMessageComplete); };
#undef _INSTANCE_CALL

  llhttp_init(&httpParser_, HTTP_REQUEST, &parsingSettings_);
  httpParser_.data = this;
}

TrInspectorClient::~TrInspectorClient()
{
  if (fd_ != -1)
    close(fd_);
}

void TrInspectorClient::tick()
{
  recv(); // recv firstly, then parse

  auto err = llhttp_execute(&httpParser_, buffer_.data(), buffer_.size());
  if (err != HPE_OK)
  {
    DEBUG(LOG_TAG_ERROR, "Failed to parse the HTTP message: %s", llhttp_errno_name(err));
    shouldClose_ = true;
  }
}

void TrInspectorClient::respond(http::Response response)
{
  static string crlf = "\r\n";
  string data = "HTTP/1.1 " + to_string(response.status.code) + " " + response.status.reason + crlf;

  auto &headerFields = response.headerFields;
  headerFields.push_back(http::HeaderField{"Server", "JSAR Inspector Server"});
  {
    auto now = chrono::system_clock::now();
    auto nowTime = chrono::system_clock::to_time_t(now);
    char timeStr[64];
    strftime(timeStr, sizeof(timeStr), "%a, %d %b %Y %H:%M:%S %Z", gmtime(&nowTime));
    headerFields.push_back(http::HeaderField{"Date", timeStr});
  }

  for (const auto &header : headerFields)
    data += header.first + ": " + header.second + "\n";

  data += crlf;
  if (response.body.size() > 0)
    data += string(response.body.begin(), response.body.end());

  // append the end of the response
  data += crlf;

  // send the response and close the connection
  send(data);
  end();
}

void TrInspectorClient::respond(uint32_t code, const std::string &text)
{
  http::Response res;
  res.status.code = code;
  res.status.reason = "OK";
  res.headerFields.push_back({"Content-Type", "text/plain"});
  res.headerFields.push_back({"Content-Length", to_string(text.size())});
  res.body.assign(text.begin(), text.end());
  respond(res);
}

void TrInspectorClient::respond(uint32_t code, const rapidjson::Document &json)
{
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  json.Accept(writer);

  http::Response res;
  res.status.code = code;
  res.status.reason = "OK";
  res.headerFields.push_back({"Content-Type", "application/json"});
  res.headerFields.push_back({"Content-Length", to_string(buffer.GetSize())});
  
  std::span<char> body(const_cast<char*>(buffer.GetString()), buffer.GetSize());
  res.body.assign(body.begin(), body.end());
  respond(res);
}

bool TrInspectorClient::setNonBlocking()
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

void TrInspectorClient::recv()
{
  char chunk[1024];
  while (true)
  {
    ssize_t bytesReceived = ::recv(fd_, chunk, sizeof(chunk), 0);
    if (bytesReceived > 0)
    {
      buffer_.insert(buffer_.end(), chunk, chunk + bytesReceived);
    }
    else if (bytesReceived == 0)
    {
      shouldClose_ = true;
      break;
    }
    else
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      else
        DEBUG(LOG_TAG_ERROR, "Failed to read data from the client(%d): %s", fd_, strerror(errno));
    }
  }
}

void TrInspectorClient::send(const string &data)
{
  ssize_t bytesSent = ::send(fd_, data.c_str(), data.size(), 0);
  if (bytesSent == -1)
    DEBUG(LOG_TAG_ERROR, "Failed to send data to the client(%d): %s", fd_, strerror(errno));
}

void TrInspectorClient::end()
{
  ::shutdown(fd_, SHUT_RDWR);
  ::close(fd_);
  fd_ = -1;
  shouldClose_ = true;
}

void TrInspectorClient::onUrl(const char *at, size_t length)
{
  url_.append(at, length);
}

void TrInspectorClient::onHeaderField(const char *at, size_t length)
{
  currentHeaderField_.append(at, length);
}

void TrInspectorClient::onHeaderFieldComplete()
{
}

void TrInspectorClient::onHeaderValue(const char *at, size_t length)
{
  currentHeaderValue_.append(at, length);
}

void TrInspectorClient::onHeaderValueComplete()
{
  headers_.emplace_back(currentHeaderField_, currentHeaderValue_);
  currentHeaderField_.clear();
  currentHeaderValue_.clear();
}

void TrInspectorClient::onMessageComplete()
{
  auto inspector = inspector_.lock();
  if (inspector == nullptr)
    end();
  else
    inspector->onRequest(*this);
}
