#include <stdexcept>
#include <span>
#include <ostream>
#include <common/debug.hpp>
#include <common/utility.hpp>
#include <sstream>
#include <cstring>
#include <algorithm>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>

// For WebSocket key generation (simple implementation without OpenSSL)
#include <array>
#include <iomanip>

#include "./inspector_client.hpp"
#include "../inspector.hpp"

using namespace std;

TrInspectorClient::TrInspectorClient(int fd, shared_ptr<TrInspector> inspector)
    : fd_(fd)
    , inspector_(inspector)
{
  struct linger lingerOpt = {1, 1};
  if (setsockopt(fd_, SOL_SOCKET, SO_LINGER, &lingerOpt, sizeof(lingerOpt)) == -1)
  {
    DEBUG(LOG_TAG_ERROR, "Failed to set the linger option: %s", strerror(errno));
    throw runtime_error("Failed to set the linger option");
  }

  if (setNonBlocking() == false)
    throw runtime_error("Failed to set the socket to non-blocking mode");

#define _INSTANCE_CALL(method, ...)                                      \
  if (parser != nullptr)                                                 \
    static_cast<TrInspectorClient *>(parser->data)->method(__VA_ARGS__); \
  return 0;

  parsingSettings_.on_url = [](llhttp_t *parser, const char *at, size_t length) -> int
  { _INSTANCE_CALL(onUrl, at, length); };
  parsingSettings_.on_url_complete = [](llhttp_t *parser) -> int
  { _INSTANCE_CALL(onUrlComplete); };
  parsingSettings_.on_status = [](llhttp_t *parser, const char *at, size_t length) -> int
  { _INSTANCE_CALL(onStatus, at, length); };
  parsingSettings_.on_status_complete = [](llhttp_t *parser) -> int
  { _INSTANCE_CALL(onStatusComplete); };
  parsingSettings_.on_method = [](llhttp_t *parser, const char *at, size_t length) -> int
  { _INSTANCE_CALL(onMethod, at, length); };
  parsingSettings_.on_method_complete = [](llhttp_t *parser) -> int
  { _INSTANCE_CALL(onMethodComplete); };
  parsingSettings_.on_version = [](llhttp_t *parser, const char *at, size_t length) -> int
  { _INSTANCE_CALL(onVersion, at, length); };
  parsingSettings_.on_version_complete = [](llhttp_t *parser) -> int
  { _INSTANCE_CALL(onVersionComplete); };
  parsingSettings_.on_header_field = [](llhttp_t *parser, const char *at, size_t length) -> int
  { _INSTANCE_CALL(onHeaderField, at, length); };
  parsingSettings_.on_header_field_complete = [](llhttp_t *parser) -> int
  { _INSTANCE_CALL(onHeaderFieldComplete); };
  parsingSettings_.on_header_value = [](llhttp_t *parser, const char *at, size_t length) -> int
  { _INSTANCE_CALL(onHeaderValue, at, length); };
  parsingSettings_.on_header_value_complete = [](llhttp_t *parser) -> int
  { _INSTANCE_CALL(onHeaderValueComplete); };
  parsingSettings_.on_headers_complete = [](llhttp_t *parser) -> int
  { _INSTANCE_CALL(onHeadersComplete); };
  parsingSettings_.on_message_begin = [](llhttp_t *parser) -> int
  { _INSTANCE_CALL(onMessageBegin); };
  parsingSettings_.on_message_complete = [](llhttp_t *parser) -> int
  { _INSTANCE_CALL(onMessageComplete); };
  parsingSettings_.on_reset = [](llhttp_t *parser) -> int
  { _INSTANCE_CALL(onReset); };
#undef _INSTANCE_CALL

  // Reset the callback functions to NULL to avoid dangling pointers
  parsingSettings_.on_chunk_extension_name = NULL;
  parsingSettings_.on_chunk_extension_value = NULL;
  parsingSettings_.on_body = NULL;
  parsingSettings_.on_chunk_extension_name_complete = NULL;
  parsingSettings_.on_chunk_extension_value_complete = NULL;
  parsingSettings_.on_chunk_header = NULL;
  parsingSettings_.on_chunk_complete = NULL;

  llhttp_init(&httpParser_, HTTP_REQUEST, &parsingSettings_);
  httpParser_.data = this;
}

TrInspectorClient::~TrInspectorClient()
{
  if (fd_ != -1)
  {
    close(fd_);
    DEBUG(LOG_TAG_INSPECTOR,
          "Inspector client fd(%d) is closed in destructor.",
          fd_);
  }
}

void TrInspectorClient::tick()
{
  recv(); // recv firstly, then parse

  if (connectionType_ == ConnectionType::WEBSOCKET)
  {
    handleWebSocketFrame();
  }
  else if (connectionType_ == ConnectionType::HTTP)
  {
    auto err = llhttp_execute(&httpParser_, buffer_.data(), buffer_.size());
    if (err == HPE_PAUSED_UPGRADE)
    {
      if (connectionType_ != ConnectionType::WEBSOCKET) [[unlikely]]
      {
        DEBUG(LOG_TAG_ERROR,
              "Received HPE_PAUSED_UPGRADE but the connection type is not WEBSOCKET, this should not happen.");
        DEBUG(LOG_TAG_ERROR, "%s %s HTTP/1.1", methodStr_.c_str(), url_.c_str());
        DEBUG(LOG_TAG_ERROR, "%s", buffer_.data());

        shouldClose_ = true;
        return;
      }
      llhttp_resume_after_upgrade(&httpParser_);
    }
    else if (err != HPE_OK)
    {
      string incomingText(buffer_.begin(), buffer_.end());
      DEBUG(LOG_TAG_ERROR,
            "Failed to parse the HTTP message, the error is: %s, and the message: %s\n",
            llhttp_errno_name(err),
            incomingText.c_str());
      shouldClose_ = true;
    }
  }
  else [[unlikely]]
  {
    assert(false && "Unknown connection type");
  }
}

void TrInspectorClient::respond(http::Response response)
{
  static string CRLF = "\r\n";
  stringstream bufferToSend;
  bufferToSend << "HTTP/1.1 " << response.status.code << " " << response.status.reason << CRLF;

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
    bufferToSend << header.first << ": " << header.second << CRLF;
  bufferToSend << CRLF;

  if (response.body.size() > 0)
  {
    const vector<unsigned char> &body = response.body;
    bufferToSend.write(reinterpret_cast<const char *>(body.data()), body.size());
  }

  // send the response and close the connection
  send(bufferToSend.str());
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
  res.headerFields.push_back({"Cache-Control", "no-cache"});
  res.headerFields.push_back({"Content-Type", "application/json; charset=UTF-8"});
  res.headerFields.push_back({"Content-Length", to_string(buffer.GetSize())});

  std::span<char> body(const_cast<char *>(buffer.GetString()), buffer.GetSize());
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

void TrInspectorClient::onUrlComplete()
{
}

void TrInspectorClient::onStatus(const char *at, size_t length)
{
}

void TrInspectorClient::onStatusComplete()
{
}

void TrInspectorClient::onMethod(const char *at, size_t length)
{
  methodStr_.append(at, length);
}

void TrInspectorClient::onMethodComplete()
{
  if (methodStr_ == "GET")
    method_ = GET;
  else if (methodStr_ == "POST")
    method_ = POST;
  else if (methodStr_ == "PUT")
    method_ = PUT;
  else if (methodStr_ == "DELETE")
    method_ = DELETE;
  else if (methodStr_ == "PATCH")
    method_ = PATCH;
  else if (methodStr_ == "OPTIONS")
    method_ = OPTIONS;
  else if (methodStr_ == "HEAD")
    method_ = HEAD;
  else
    method_ = GET; // default to GET
}

void TrInspectorClient::onVersion(const char *at, size_t length)
{
}

void TrInspectorClient::onVersionComplete()
{
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

void TrInspectorClient::onHeadersComplete()
{
}

void TrInspectorClient::onMessageBegin()
{
}

void TrInspectorClient::onMessageComplete()
{
  // Try to upgrade to WebSocket if requested
  if (tryUpgradeToWebSocket())
  {
    return; // Connection upgraded, don't process as HTTP
  }

  auto inspector = inspector_.lock();
  if (inspector == nullptr)
    end();
  else
    inspector->onRequest(*this);
}

void TrInspectorClient::onReset()
{
  DEBUG(LOG_TAG_ERROR, "Request reset");
  end();
}

bool TrInspectorClient::tryUpgradeToWebSocket()
{
  // Check if this is a WebSocket upgrade request
  bool hasUpgrade = false;
  bool hasConnection = false;
  bool hasWebSocketKey = false;
  string webSocketKey;

  for (const auto &header : headers_)
  {
    string lowerKey = header.first;
    transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);

    string lowerValue = header.second;
    transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);

    if (lowerKey == "upgrade" && lowerValue == "websocket")
    {
      hasUpgrade = true;
    }
    else if (lowerKey == "connection")
    {
      if (lowerValue.find("upgrade") != string::npos)
      {
        hasConnection = true;
      }
    }
    else if (lowerKey == "sec-websocket-key")
    {
      hasWebSocketKey = true;
      webSocketKey = header.second;
    }
  }

  // Only allow WebSocket upgrade for /devtools/inspector/:client paths
  if (hasUpgrade && hasConnection && hasWebSocketKey)
  {
    // Validate URL path - must match /devtools/inspector/:client pattern
    if (!url_.starts_with("/devtools/inspector/"))
    {
      DEBUG(LOG_TAG_INSPECTOR, "WebSocket upgrade rejected: invalid path '%s'", url_.c_str());
      sendHttpErrorResponse(404, "WebSocket upgrades only supported on /devtools/inspector/:client");
      return true; // We handled the request, even though we rejected it
    }

    // Extract client ID from URL
    string clientId = url_.substr(20); // Skip "/devtools/inspector/"
    if (clientId.empty())
    {
      DEBUG(LOG_TAG_INSPECTOR, "WebSocket upgrade rejected: missing client ID in path '%s'", url_.c_str());
      sendHttpErrorResponse(400, "Missing client ID in WebSocket URL");
      return true;
    }

    // Store the client ID for later use
    clientId_ = clientId;
    DEBUG(LOG_TAG_INSPECTOR, "WebSocket upgrade requested for client '%s'", clientId.c_str());

    // Check WebSocket connection limit
    auto inspector = inspector_.lock();
    if (inspector == nullptr)
    {
      return false;
    }

    if (!inspector->canAcceptWebSocketConnection())
    {
      DEBUG(LOG_TAG_INSPECTOR, "WebSocket connection limit reached, rejecting upgrade");
      sendHttpErrorResponse(503, "Too many WebSocket connections (max 5)");
      return true; // We handled the request, even though we rejected it
    }

    DEBUG(LOG_TAG_INSPECTOR, "WebSocket upgrade approved for client '%s'", clientId.c_str());

    // Generate WebSocket accept key
    string acceptKey = generateWebSocketAcceptKey(webSocketKey);

    // Send WebSocket handshake response
    stringstream response;
    response << "HTTP/1.1 101 Switching Protocols\r\n";
    response << "Upgrade: websocket\r\n";
    response << "Connection: Upgrade\r\n";
    response << "Sec-WebSocket-Accept: " << acceptKey << "\r\n";
    response << "\r\n";

    send(response.str());

    // Switch to WebSocket mode
    connectionType_ = ConnectionType::WEBSOCKET;
    buffer_.clear(); // Clear HTTP parsing buffer

    DEBUG(LOG_TAG_INSPECTOR, "WebSocket connection upgraded successfully");
    return true;
  }

  return false;
}

std::string TrInspectorClient::generateWebSocketAcceptKey(const std::string &webSocketKey)
{
  // WebSocket magic string as per RFC 6455
  const std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  std::string combined = webSocketKey + magic;

  // Simple SHA-1 implementation (minimal for WebSocket compliance)
  auto sha1 = [](const std::string &input) -> std::array<uint8_t, 20>
  {
    // This is a simplified SHA-1 implementation for demonstration
    // In production, you might want to use a proper crypto library
    std::array<uint8_t, 20> hash = {};

    // Initialize hash values
    uint32_t h0 = 0x67452301;
    uint32_t h1 = 0xEFCDAB89;
    uint32_t h2 = 0x98BADCFE;
    uint32_t h3 = 0x10325476;
    uint32_t h4 = 0xC3D2E1F0;

    // Pre-processing
    std::vector<uint8_t> message(input.begin(), input.end());
    size_t originalLength = message.size();

    // Append bit '1'
    message.push_back(0x80);

    // Append zeros
    while ((message.size() % 64) != 56)
    {
      message.push_back(0x00);
    }

    // Append original length as 64-bit big-endian
    uint64_t bitLength = originalLength * 8;
    for (int i = 7; i >= 0; i--)
    {
      message.push_back((bitLength >> (i * 8)) & 0xFF);
    }

    // Process message in 512-bit chunks
    for (size_t chunk = 0; chunk < message.size(); chunk += 64)
    {
      std::array<uint32_t, 80> w = {};

      // Break chunk into sixteen 32-bit big-endian words
      for (int i = 0; i < 16; i++)
      {
        w[i] = (static_cast<uint32_t>(message[chunk + i * 4]) << 24) |
               (static_cast<uint32_t>(message[chunk + i * 4 + 1]) << 16) |
               (static_cast<uint32_t>(message[chunk + i * 4 + 2]) << 8) |
               static_cast<uint32_t>(message[chunk + i * 4 + 3]);
      }

      // Extend the sixteen 32-bit words into eighty 32-bit words
      for (int i = 16; i < 80; i++)
      {
        uint32_t temp = w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16];
        w[i] = (temp << 1) | (temp >> 31);
      }

      // Initialize hash value for this chunk
      uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;

      // Main loop
      for (int i = 0; i < 80; i++)
      {
        uint32_t f, k;
        if (i < 20)
        {
          f = (b & c) | ((~b) & d);
          k = 0x5A827999;
        }
        else if (i < 40)
        {
          f = b ^ c ^ d;
          k = 0x6ED9EBA1;
        }
        else if (i < 60)
        {
          f = (b & c) | (b & d) | (c & d);
          k = 0x8F1BBCDC;
        }
        else
        {
          f = b ^ c ^ d;
          k = 0xCA62C1D6;
        }

        uint32_t temp = ((a << 5) | (a >> 27)) + f + e + k + w[i];
        e = d;
        d = c;
        c = (b << 30) | (b >> 2);
        b = a;
        a = temp;
      }

      // Add this chunk's hash to result
      h0 += a;
      h1 += b;
      h2 += c;
      h3 += d;
      h4 += e;
    }

    // Produce the final hash value as a 160-bit number (20 bytes)
    for (int i = 0; i < 4; i++)
    {
      hash[i] = (h0 >> (24 - i * 8)) & 0xFF;
      hash[i + 4] = (h1 >> (24 - i * 8)) & 0xFF;
      hash[i + 8] = (h2 >> (24 - i * 8)) & 0xFF;
      hash[i + 12] = (h3 >> (24 - i * 8)) & 0xFF;
      hash[i + 16] = (h4 >> (24 - i * 8)) & 0xFF;
    }

    return hash;
  };

  // Compute SHA-1 hash
  auto hash = sha1(combined);

  // Base64 encode the hash using utility function
  return Base64Encode(hash);
}

void TrInspectorClient::handleWebSocketFrame()
{
  if (buffer_.size() < 2)
  {
    return; // Not enough data for frame header
  }

  // Basic WebSocket frame parsing (simplified for text frames)
  uint8_t firstByte = static_cast<uint8_t>(buffer_[0]);
  uint8_t secondByte = static_cast<uint8_t>(buffer_[1]);

  bool fin = (firstByte & 0x80) != 0;
  uint8_t opcode = firstByte & 0x0F;
  bool masked = (secondByte & 0x80) != 0;
  uint64_t payloadLength = secondByte & 0x7F;

  size_t frameHeaderSize = 2;

  // Handle extended payload length
  if (payloadLength == 126)
  {
    if (buffer_.size() < 4)
      return;
    payloadLength = (static_cast<uint8_t>(buffer_[2]) << 8) | static_cast<uint8_t>(buffer_[3]);
    frameHeaderSize = 4;
  }
  else if (payloadLength == 127)
  {
    if (buffer_.size() < 10)
      return;
    // For simplicity, we don't handle 64-bit payload lengths
    DEBUG(LOG_TAG_ERROR, "WebSocket frame with 64-bit payload length not supported");
    shouldClose_ = true;
    return;
  }

  if (masked)
  {
    frameHeaderSize += 4; // mask key
  }

  if (buffer_.size() < frameHeaderSize + payloadLength)
  {
    return; // Not enough data for complete frame
  }

  // Handle close frame
  if (opcode == 0x8)
  {
    shouldClose_ = true;
    return;
  }

  // Handle text frame
  if (opcode == 0x1)
  {
    std::vector<char> payload(payloadLength);
    size_t payloadStart = frameHeaderSize;

    if (masked)
    {
      // Extract mask key
      uint8_t maskKey[4];
      for (int i = 0; i < 4; i++)
      {
        maskKey[i] = static_cast<uint8_t>(buffer_[frameHeaderSize - 4 + i]);
      }

      // Unmask payload
      for (size_t i = 0; i < payloadLength; i++)
      {
        payload[i] = buffer_[payloadStart + i] ^ maskKey[i % 4];
      }
    }
    else
    {
      std::copy(buffer_.begin() + payloadStart, buffer_.begin() + payloadStart + payloadLength, payload.begin());
    }

    string message(payload.begin(), payload.end());

    // Forward message to inspector for handling
    auto inspector = inspector_.lock();
    if (inspector)
    {
      inspector->onMessage(*this, message);
    }
  }

  // Remove processed frame from buffer
  buffer_.erase(buffer_.begin(), buffer_.begin() + frameHeaderSize + payloadLength);
}

void TrInspectorClient::sendWebSocketFrame(const string &data)
{
  vector<uint8_t> frame;

  // First byte: FIN=1, RSV=000, Opcode=0001 (text frame)
  frame.push_back(0x81);

  // Payload length
  if (data.length() < 126)
  {
    frame.push_back(static_cast<uint8_t>(data.length()));
  }
  else if (data.length() < 65536)
  {
    frame.push_back(126);
    frame.push_back(static_cast<uint8_t>((data.length() >> 8) & 0xFF));
    frame.push_back(static_cast<uint8_t>(data.length() & 0xFF));
  }
  else
  {
    // For simplicity, don't handle very large messages
    DEBUG(LOG_TAG_ERROR, "WebSocket message too large");
    return;
  }

  // Add payload
  frame.insert(frame.end(), data.begin(), data.end());

  // Send frame
  ssize_t bytesSent = ::send(fd_, frame.data(), frame.size(), 0);
  if (bytesSent == -1)
  {
    DEBUG(LOG_TAG_ERROR, "Failed to send WebSocket frame to client(%d): %s", fd_, strerror(errno));
  }
}

void TrInspectorClient::sendWebSocketMessage(const string &message)
{
  if (connectionType_ == ConnectionType::WEBSOCKET)
  {
    sendWebSocketFrame(message);
  }
}

void TrInspectorClient::sendHttpErrorResponse(uint32_t code, const string &message)
{
  stringstream response;
  response << "HTTP/1.1 " << code;

  switch (code)
  {
  case 400:
    response << " Bad Request\r\n";
    break;
  case 503:
    response << " Service Unavailable\r\n";
    break;
  default:
    response << " Error\r\n";
    break;
  }

  response << "Content-Type: text/plain\r\n";
  response << "Content-Length: " << message.length() << "\r\n";
  response << "\r\n";
  response << message;

  send(response.str());
  end();
}
