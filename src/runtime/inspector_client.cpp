#include <stdexcept>
#include <span>
#include <ostream>
#include <common/debug.hpp>
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

// For WebSocket key generation
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include "./inspector_client.hpp"
#include "./inspector.hpp"

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
    close(fd_);
}

void TrInspectorClient::tick()
{
  recv(); // recv firstly, then parse

  if (connectionType_ == ConnectionType::WEBSOCKET) {
    handleWebSocketFrame();
    return;
  }

  auto err = llhttp_execute(&httpParser_, buffer_.data(), buffer_.size());
  if (err != HPE_OK)
  {
    string incomingText(buffer_.begin(), buffer_.end());
    DEBUG(LOG_TAG_ERROR, "Failed to parse the HTTP message, the error is: %s, and the message: %s\n", llhttp_errno_name(err), incomingText.c_str());
    shouldClose_ = true;
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
  if (tryUpgradeToWebSocket()) {
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
  std::string webSocketKey;

  for (const auto& header : headers_) {
    std::string lowerKey = header.first;
    std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);
    
    if (lowerKey == "upgrade" && header.second == "websocket") {
      hasUpgrade = true;
    } else if (lowerKey == "connection") {
      std::string lowerValue = header.second;
      std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
      if (lowerValue.find("upgrade") != std::string::npos) {
        hasConnection = true;
      }
    } else if (lowerKey == "sec-websocket-key") {
      hasWebSocketKey = true;
      webSocketKey = header.second;
    }
  }

  if (hasUpgrade && hasConnection && hasWebSocketKey) {
    // Check WebSocket connection limit
    auto inspector = inspector_.lock();
    if (inspector != nullptr) {
      // For simplicity, we'll implement the connection count check in the inspector
      // In a more complete implementation, we'd check the server's connection count
      DEBUG(LOG_TAG_INSPECTOR, "WebSocket upgrade requested, checking connection limit");
    }
    
    // Generate WebSocket accept key
    std::string acceptKey = generateWebSocketAcceptKey(webSocketKey);
    
    // Send WebSocket handshake response
    std::stringstream response;
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
  
  // Compute SHA-1 hash
  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1(reinterpret_cast<const unsigned char*>(combined.c_str()), combined.length(), hash);
  
  // Base64 encode the hash
  BIO *bio, *b64;
  BUF_MEM *bufferPtr;
  
  b64 = BIO_new(BIO_f_base64());
  bio = BIO_new(BIO_s_mem());
  bio = BIO_push(b64, bio);
  
  BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
  BIO_write(bio, hash, SHA_DIGEST_LENGTH);
  BIO_flush(bio);
  BIO_get_mem_ptr(bio, &bufferPtr);
  
  std::string result(bufferPtr->data, bufferPtr->length);
  BIO_free_all(bio);
  
  return result;
}

void TrInspectorClient::handleWebSocketFrame()
{
  if (buffer_.size() < 2) {
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
  if (payloadLength == 126) {
    if (buffer_.size() < 4) return;
    payloadLength = (static_cast<uint8_t>(buffer_[2]) << 8) | static_cast<uint8_t>(buffer_[3]);
    frameHeaderSize = 4;
  } else if (payloadLength == 127) {
    if (buffer_.size() < 10) return;
    // For simplicity, we don't handle 64-bit payload lengths
    DEBUG(LOG_TAG_ERROR, "WebSocket frame with 64-bit payload length not supported");
    shouldClose_ = true;
    return;
  }
  
  if (masked) {
    frameHeaderSize += 4; // mask key
  }
  
  if (buffer_.size() < frameHeaderSize + payloadLength) {
    return; // Not enough data for complete frame
  }
  
  // Handle close frame
  if (opcode == 0x8) {
    shouldClose_ = true;
    return;
  }
  
  // Handle text frame
  if (opcode == 0x1) {
    std::vector<char> payload(payloadLength);
    size_t payloadStart = frameHeaderSize;
    
    if (masked) {
      // Extract mask key
      uint8_t maskKey[4];
      for (int i = 0; i < 4; i++) {
        maskKey[i] = static_cast<uint8_t>(buffer_[frameHeaderSize - 4 + i]);
      }
      
      // Unmask payload
      for (size_t i = 0; i < payloadLength; i++) {
        payload[i] = buffer_[payloadStart + i] ^ maskKey[i % 4];
      }
    } else {
      std::copy(buffer_.begin() + payloadStart, buffer_.begin() + payloadStart + payloadLength, payload.begin());
    }
    
    std::string message(payload.begin(), payload.end());
    DEBUG(LOG_TAG_INSPECTOR, "Received WebSocket message: %s", message.c_str());
    
    // For now, just echo the message back (placeholder for CDP implementation)
    sendWebSocketMessage("Echo: " + message);
  }
  
  // Remove processed frame from buffer
  buffer_.erase(buffer_.begin(), buffer_.begin() + frameHeaderSize + payloadLength);
}

void TrInspectorClient::sendWebSocketFrame(const std::string &data)
{
  std::vector<uint8_t> frame;
  
  // First byte: FIN=1, RSV=000, Opcode=0001 (text frame)
  frame.push_back(0x81);
  
  // Payload length
  if (data.length() < 126) {
    frame.push_back(static_cast<uint8_t>(data.length()));
  } else if (data.length() < 65536) {
    frame.push_back(126);
    frame.push_back(static_cast<uint8_t>((data.length() >> 8) & 0xFF));
    frame.push_back(static_cast<uint8_t>(data.length() & 0xFF));
  } else {
    // For simplicity, don't handle very large messages
    DEBUG(LOG_TAG_ERROR, "WebSocket message too large");
    return;
  }
  
  // Add payload
  frame.insert(frame.end(), data.begin(), data.end());
  
  // Send frame
  ssize_t bytesSent = ::send(fd_, frame.data(), frame.size(), 0);
  if (bytesSent == -1) {
    DEBUG(LOG_TAG_ERROR, "Failed to send WebSocket frame to client(%d): %s", fd_, strerror(errno));
  }
}

void TrInspectorClient::sendWebSocketMessage(const std::string &message)
{
  if (connectionType_ == ConnectionType::WEBSOCKET) {
    sendWebSocketFrame(message);
  }
}
